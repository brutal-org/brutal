#include <brutal/base.h>
#include <brutal/log.h>
#include "kernel/mmio.h"
#include "kernel/x86_64/acpi.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/apic/timer.h"
#include "kernel/x86_64/cpu.h"
#include "kernel/x86_64/memory/mmap.h"
#include "kernel/x86_64/msr.h"
#include "kernel/x86_64/pic.h"

/* --- Lapic ---------------------------------------------------------------- */

static uintptr_t lapic_base = 0;

uint32_t lapic_read(uint32_t reg)
{
    return mmio_read32((uintptr_t)lapic_base + reg);
}

void lapic_write(uint32_t reg, uint32_t value)
{
    mmio_write32(lapic_base + reg, value);
}

void lapic_enable_spurious(void)
{
    lapic_write(LAPIC_REG_SPURIOUS, lapic_read(LAPIC_REG_SPURIOUS) | (LAPIC_SPURIOUS_ALL | LAPIC_SPURIOUS_ENABLE_APIC));
}

void lapic_initialize(Handover const *handover)
{
    lapic_base = mmap_phys_to_io(acpi_find_lapic(handover->rsdp));
    log("Lapic found at {p}", lapic_base);

    auto lapics = acpi_find_lapic_table(handover->rsdp);

    for (size_t i = 0; i < lapics.count; i++)
    {
        auto lapic = lapics.table[i];
        cpu_found(lapic->processor_id, lapic->id);
    }
}

/* --- Ioapic --------------------------------------------------------------- */

struct ioapic_record_table ioapic_table = {};

static inline uint32_t ioapic_read(int index, uint32_t reg)
{
    auto base = mmap_phys_to_io(ioapic_table.table[index]->address);

    mmio_write32(base + IOAPIC_REG_OFFSET, reg);
    return mmio_read32(base + IOAPIC_VALUE_OFFSET);
}

static inline void ioapic_write(int index, uint32_t reg, uint32_t value)
{
    auto base = mmap_phys_to_io(ioapic_table.table[index]->address);

    mmio_write32(base + IOAPIC_REG_OFFSET, reg);
    mmio_write32(base + IOAPIC_VALUE_OFFSET, value);
}

struct ioapic_version ioapic_get_version(int index)
{
    uint32_t raw = ioapic_read(index, IOAPIC_REG_VERSION);

    return union_cast(struct ioapic_version, raw);
}

void ioapic_initialize(Handover const *handover)
{
    ioapic_table = acpi_find_ioapic_table(handover->rsdp);

    for (size_t i = 0; i < ioapic_table.count; i++)
    {
        auto ioapic = ioapic_table.table[i];

        auto version = ioapic_get_version(i);

        log("Ioapic {} found:", i);
        log(" - Address: {#p}", ioapic->address);
        log(" - Interrupt base: {}", ioapic->interrupt_base);
        log(" - Version: {}", version.version);
        log(" - Max redirection: {}", version.maximum_redirection);
    }
}

/* --- Apic ----------------------------------------------------------------- */

void apic_initalize(Handover const *handover)
{
    lapic_initialize(handover);
    ioapic_initialize(handover);

    apic_enable();

    apic_init_interrupt_redirection(handover);
    apic_timer_initialize();
}

void apic_eoi(void)
{
    lapic_write(LAPIC_REG_EOI, 0);
}

void apic_send_ipit(CpuId cpu_id, uint32_t interrupt_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, interrupt_id | LAPIC_ICR_CLEAR_INIT_LEVEL);
}

void apic_init_processor(CpuId cpu_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_INIT);
}

void apic_start_processor(CpuId cpu_id, uintptr_t entry)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_SEND_IPI | (entry / 4096));
}

CpuId apic_current_cpu(void)
{
    if (lapic_base == 0)
    {
        return 0;
    }
    else
    {
        return lapic_read(LAPIC_CPU_ID) >> 24;
    }
}

void apic_enable(void)
{
    wrmsr(MSR_APIC, (rdmsr(MSR_APIC) | LAPIC_ENABLE) & ~((1 << 10)));
    lapic_enable_spurious();
}

/* --- Ioapic interrupt redirection --------------------------------------------------------------- */

typedef Range(uint8_t) IoapicRange;

static IoApicRedirectResult find_ioapic_for_interrupt_base(uint32_t interrupt_base)
{
    for (size_t i = 0; i < ioapic_table.count; i++)
    {
        IoapicRange interrupt_range = {
            .base = ioapic_table.table[i]->interrupt_base,
            .size = ioapic_get_version(i).maximum_redirection,
        };

        if (range_contain(interrupt_range, interrupt_base))
        {
            return OK(IoApicRedirectResult, i);
        }
    }

    log("warning: ioapic interrupt base not founded for base {}", interrupt_base);
    return ERR(IoApicRedirectResult, 0);
}

static IoApicRedirectResult apic_create_interrupt_redirect(uint8_t interrupt_id, uint32_t interrupt_base, uint16_t flags, int cpu, bool enable)
{
    int ioapic_target = TRY(IoApicRedirectResult, find_ioapic_for_interrupt_base(interrupt_base));

    struct ioapic_ipit_redirection_entry redirect = {};

    redirect.interrupt = interrupt_id;

    if (flags & IPIT_FLAGS_ACTIVE_HIGH_LOW)
    {
        redirect.pin_polarity = 1;
    }

    if (flags & IPIT_EDGE_LOW)
    {
        redirect.trigger_mode = 1;
    }

    if (!enable)
    {
        redirect.mask = 1;
    }

    redirect.destination = cpu_impl(cpu)->lapic;

    // for each apic redirection register we have:
    // [0x10] IOAPIC_REG_REDIRECT_BASE
    // [0x10] interrupt 0 reg 1
    // [0x11] interrupt 0 reg 2
    // [0x12] interrupt 1 reg 1
    // [0x13] interrupt 1 reg 2
    // ...

    uint32_t ioapic_table_interrupt_offset = interrupt_base - ioapic_table.table[ioapic_target]->interrupt_base;

    uint32_t target_io_offset = ioapic_table_interrupt_offset * 2;

    ioapic_write(ioapic_target, IOAPIC_REG_REDIRECT_BASE + target_io_offset, redirect._low_byte);
    ioapic_write(ioapic_target, IOAPIC_REG_REDIRECT_BASE + target_io_offset + 1, redirect._high_byte);

    return OK(IoApicRedirectResult, 0);
}

IoApicRedirectResult apic_redirect_irq_to_cpu(CpuId id, uint8_t irq, bool enable, uintptr_t rsdp)
{
    struct iso_record_table iso_table = acpi_find_iso_table(rsdp);

    log("- apic: setting ipit {#p} redirection for cpu: {}", irq, id);

    for (size_t i = 0; i < iso_table.count; i++)
    {
        if (iso_table.table[i]->irq == irq)
        {
            log("   - apic: using iso: {} with interrupt base: {}", i, iso_table.table[i]->interrupt_base);

            TRY(IoApicRedirectResult,
                apic_create_interrupt_redirect(iso_table.table[i]->irq + 0x20, iso_table.table[i]->interrupt_base, iso_table.table[i]->flags, id, enable));

            return OK(IoApicRedirectResult, iso_table.table[i]->interrupt_base);
        }
    }

    TRY(IoApicRedirectResult, apic_create_interrupt_redirect(irq + 0x20, irq, 0, id, enable));

    return OK(IoApicRedirectResult, irq);
}

IoApicRedirectResult apic_init_interrupt_redirection(Handover const *handover)
{
    log("apic: loading ipit redirection");

    for (size_t i = 0; i < 16; i++) // set interrupt 32 to 48
    {
        TRY(IoApicRedirectResult, apic_redirect_irq_to_cpu(apic_current_cpu(), i, true, handover->rsdp));
    }

    return OK(IoApicRedirectResult, 0);
}
