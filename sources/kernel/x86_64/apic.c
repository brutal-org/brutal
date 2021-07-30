#include <brutal/base.h>
#include <brutal/log.h>
#include <brutal/mem.h>
#include "kernel/mmap.h"
#include "kernel/x86_64/acpi.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/cpu.h"
#include "kernel/x86_64/hpet.h"
#include "kernel/x86_64/pic.h"

/* --- Lapic ---------------------------------------------------------------- */

static uintptr_t lapic_base = 0;

uint32_t lapic_read(uint32_t reg)
{
    return volatile_read32((uintptr_t)lapic_base + reg);
}

void lapic_write(uint32_t reg, uint32_t value)
{
    volatile_write32(lapic_base + reg, value);
}

void lapic_eoi(void)
{
    lapic_write(LAPIC_REG_EOI, 0);
}

void lapic_send_ipi(CpuId cpu_id, uint32_t interrupt_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, interrupt_id | LAPIC_ICR_CLEAR_INIT_LEVEL);
}

void lapic_send_init(CpuId cpu_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_INIT);
}

void lapic_send_sipi(CpuId cpu_id, uintptr_t entry)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_SEND_IPI | (entry / 4096));
}

CpuId lapic_current_cpu(void)
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

void lapic_enable(void)
{
    asm_write_msr(MSR_APIC, (asm_read_msr(MSR_APIC) | LAPIC_ENABLE) & ~((1 << 10)));

    lapic_write(LAPIC_REG_SPURIOUS, lapic_read(LAPIC_REG_SPURIOUS) | (LAPIC_SPURIOUS_ALL | LAPIC_SPURIOUS_ENABLE_APIC));
}

void lapic_timer_initialize(void)
{
    lapic_write(LAPIC_REG_TIMER_DIV, APIC_TIMER_DIVIDE_BY_16);
    lapic_write(LAPIC_REG_TIMER_INITCNT, 0xFFFFFFFF); /* Set the value to -1 */

    hpet_sleep(10);

    lapic_write(LAPIC_REG_LVT_TIMER, LAPIC_TIMER_MASKED);

    uint32_t tick_in_10ms = 0xFFFFFFFF - lapic_read(LAPIC_REG_TIMER_CURRCNT);

    lapic_write(LAPIC_REG_LVT_TIMER, LAPIC_TIMER_IRQ | LAPIC_TIMER_PERIODIC);
    lapic_write(LAPIC_REG_TIMER_DIV, APIC_TIMER_DIVIDE_BY_16);
    lapic_write(LAPIC_REG_TIMER_INITCNT, tick_in_10ms / 10);
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

    volatile_write32(base + IOAPIC_REG_OFFSET, reg);
    return volatile_read32(base + IOAPIC_VALUE_OFFSET);
}

static inline void ioapic_write(int index, uint32_t reg, uint32_t value)
{
    auto base = mmap_phys_to_io(ioapic_table.table[index]->address);

    volatile_write32(base + IOAPIC_REG_OFFSET, reg);
    volatile_write32(base + IOAPIC_VALUE_OFFSET, value);
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
        log(" - Max redirection: {}", version.max_redirect);
    }
}

typedef Range(uint8_t) IoapicRange;

static int ioapic_from_gsi(uint32_t interrupt_base)
{
    for (size_t i = 0; i < ioapic_table.count; i++)
    {
        IoapicRange interrupt_range = {
            .base = ioapic_table.table[i]->interrupt_base,
            .size = ioapic_get_version(i).max_redirect,
        };

        if (range_contain(interrupt_range, interrupt_base))
        {
            return i;
        }
    }

    panic("Ioapic interrupt base not founded for base {}", interrupt_base);
}

static void ioapic_create_redirect(
    uint8_t interrupt_id,
    uint32_t interrupt_base,
    uint16_t flags,
    int cpu,
    bool enable)
{
    int ioapic_target = ioapic_from_gsi(interrupt_base);

    struct ioapic_redirect entry = {};

    entry.interrupt = interrupt_id;

    if (flags & IOAPIC_ACTIVE_HIGH_LOW)
    {
        entry.pin_polarity = 1;
    }

    if (flags & IOAPIC_TRIGGER_EDGE_LOW)
    {
        entry.trigger_mode = 1;
    }

    if (!enable)
    {
        entry.mask = 1;
    }

    entry.destination = cpu_impl(cpu)->lapic;

    uint32_t ioapic_table_interrupt_offset = interrupt_base - ioapic_table.table[ioapic_target]->interrupt_base;

    uint32_t target_io_offset = ioapic_table_interrupt_offset * 2;

    ioapic_write(ioapic_target, IOAPIC_REG_REDIRECT_BASE + target_io_offset, entry._low_byte);
    ioapic_write(ioapic_target, IOAPIC_REG_REDIRECT_BASE + target_io_offset + 1, entry._high_byte);
}

void ioapic_redirect_irq_to_cpu(CpuId id, uint8_t irq, bool enable, uintptr_t rsdp)
{
    struct iso_record_table iso_table = acpi_find_iso_table(rsdp);

    log("- apic: setting ipi {#p} redirection for cpu: {}", irq, id);

    for (size_t i = 0; i < iso_table.count; i++)
    {
        if (iso_table.table[i]->irq == irq)
        {
            log("   - apic: using iso: {} with interrupt base: {}", i, iso_table.table[i]->interrupt_base);

            ioapic_create_redirect(iso_table.table[i]->irq + 0x20, iso_table.table[i]->interrupt_base, iso_table.table[i]->flags, id, enable);

            return;
        }
    }

    ioapic_create_redirect(irq + 0x20, irq, 0, id, enable);
}

void ioapic_redirect_legacy_irq(Handover const *handover)
{
    log("IOApic: Setup irq redirection");

    for (size_t i = 0; i < 16; i++) // set interrupt 32 to 48
    {
        ioapic_redirect_irq_to_cpu(lapic_current_cpu(), i, true, handover->rsdp);
    }
}

/* --- Apic ----------------------------------------------------------------- */

void apic_initalize(Handover const *handover)
{
    lapic_initialize(handover);
    ioapic_initialize(handover);

    lapic_enable();

    ioapic_redirect_legacy_irq(handover);
    lapic_timer_initialize();
}
