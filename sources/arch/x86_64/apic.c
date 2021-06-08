#include <library/base/std.h>
#include <library/log.h>

#include "arch/x86_64/acpi.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/mmap.h"
#include "arch/x86_64/msr.h"
#include "arch/x86_64/pic.h"
#include "kernel/mmio.h"

/* --- Lapic ---------------------------------------------------------------- */

static uintptr_t lapic_base = 0;

static inline uint32_t lapic_read(uint32_t reg)
{
    return mmio_read32((uintptr_t)lapic_base + reg);
}

static inline void lapic_write(uint32_t reg, uint32_t value)
{
    mmio_write32(lapic_base + reg, value);
}

void lapic_enable_spurious(void)
{
    lapic_write(LAPIC_REG_SPURIOUS, LAPIC_SPURIOUS_ALL | LAPIC_SPURIOUS_ENABLE_APIC);
}

void lapic_initialize(struct handover const *handover)
{
    lapic_base = mmap_phys_to_io(acpi_find_lapic(handover->rsdp));
    log("Lapic found at {p}", lapic_base);

    auto lapics = acpi_find_lapic_table(handover->rsdp);

    for (size_t i = 0; i < lapics.count; i++)
    {
        auto lapic = lapics.table[i];
        cpu_context_found(lapic->processor_id, lapic->id);
    }
}

/* --- Ioapic --------------------------------------------------------------- */

static uintptr_t ioapic_base[256] = {};

static inline uint32_t ioapic_read(int ioapic_id, uint32_t reg)
{
    auto base = mmap_phys_to_io(ioapic_base[ioapic_id]);

    mmio_write32(base, reg);
    return mmio_read32(base + 16);
}

static inline void ioapic_write(int ioapic_id, uint32_t reg, uint32_t value)
{
    auto base = mmap_phys_to_io(ioapic_base[ioapic_id]);

    mmio_write32(base, reg);
    mmio_write32(base, value);
}

struct ioapic_version ioapic_get_version(int ioapic_id)
{
    uint32_t raw = ioapic_read(ioapic_id, IOAPIC_REG_VERSION);

    return *((struct ioapic_version *)(&raw));
}

void ioapic_initialize(struct handover const *handover)
{
    auto ioapics = acpi_find_ioapic_table(handover->rsdp);

    for (size_t i = 0; i < ioapics.count; i++)
    {
        auto ioapic = ioapics.table[i];
        ioapic_base[ioapic->id] = ioapic->interrupt_base;
        auto version = ioapic_get_version(ioapic->id);

        log("Ioapic {} found:", i);
        log(" - Address: {#p}", ioapic->address);
        log(" - Interrupt base: {}", ioapic->interrupt_base);
        log(" - Version: {}", version.version);
        log(" - Max redirection: {}", version.maximum_redirection);
    }
}

/* --- Apic ----------------------------------------------------------------- */

void apic_initalize(struct handover const *handover)
{
    lapic_initialize(handover);
    ioapic_initialize(handover);

    apic_enable();
    lapic_enable_spurious();
}

void apic_eoi(void)
{
    lapic_write(LAPIC_REG_EOI, 0);
}

void apic_send_ipit(cpu_id_t cpu_id, uint32_t interrupt_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, interrupt_id | LAPIC_ICR_CLEAR_INIT_LEVEL);
}

void apic_init_processor(cpu_id_t cpu_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_INIT);
}

void apic_start_processor(cpu_id_t cpu_id, uintptr_t entry)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_SEND_IPI | (entry / 4096));
}

cpu_id_t apic_current_cpu(void)
{
    return lapic_read(LAPIC_CPU_ID) >> 24;
}

void apic_enable(void)
{
    pic_disable();
    wrmsr(MSR_APIC, (rdmsr(MSR_APIC) | LAPIC_ENABLE));
}
