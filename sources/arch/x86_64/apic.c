#include <library/base/std.h>
#include <library/log.h>

#include "arch/x86_64/acpi.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/mmap.h"
#include "arch/x86_64/msr.h"
#include "arch/x86_64/pic.h"
#include "kernel/mmio.h"

static struct lapic_record_table lapic_table;
static struct ioapic_record_table ioapic_table;
static uintptr_t lapic_base;
static size_t cpu_count;

static inline uint32_t lapic_read(uint32_t reg)
{
    return mmio_read32((uintptr_t)lapic_base + reg);
}

static inline void lapic_write(uint32_t reg, uint32_t value)
{
    mmio_write32(lapic_base + reg, value);
}

static inline uint32_t ioapic_read(uintptr_t base, uint32_t reg)
{
    mmio_write32(base, reg);
    return mmio_read32(base + 16);
}

static inline void ioapic_write(uintptr_t base, uint32_t reg, uint32_t value)
{
    mmio_write32(base, reg);
    mmio_write32(base, value);
}

void apic_initialize_cpu_structure()
{
    log("lapic table:");
    for (size_t i = 0; i < lapic_table.count; i++)
    {
        log("   lapic id: {} apic id: {#p} ", i, lapic_table.table[i]->processor_id, lapic_table.table[i]->apic_id);

        cpu_count++;

        get_cpu_structure(i)->lapic_id = lapic_table.table[i]->apic_id;
        get_cpu_structure(i)->cpu_id = i;
    }
}

void ioapic_initialize()
{
    log("ioapic table:");

    for (size_t i = 0; i < ioapic_table.count; i++)
    {
        struct ioapic_version_reg version = ioapic_get_version(i);

        log("   ioapic id: {} address: {#p} interrupt base: {}", i, ioapic_table.table[i]->address, ioapic_table.table[i]->interrupt_base);
        log("   - version: {} max redirection: {}", version.version, version.maximum_redirection);
    }
}

void apic_initalize(struct handover const *handover)
{
    cpu_count = 0;

    lapic_base = mmap_phys_to_io(acpi_find_lapic(handover->rsdp));
    lapic_table = acpi_find_lapic_table(handover->rsdp);
    ioapic_table = acpi_find_ioapic_table(handover->rsdp);

    log("lapic found at {p}", lapic_base);

    apic_enable();
    lapic_enable_spurious();

    apic_initialize_cpu_structure();
    ioapic_initialize();

    log("lapic: current cpu id {#p}", apic_get_current_cpu_id());
    log("lapic: total cpu count {#p}", apic_get_processor_count());
}

void apic_eoi(void)
{
    lapic_write(LAPIC_REG_EOI, 0);
}

void apic_send_interrupt(uint8_t cpu_id, uint32_t interrupt_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, interrupt_id | LAPIC_ICR_CLEAR_INIT_LEVEL);
}

void apic_init_processor(uint32_t cpu_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_INIT);
}

void apic_start_processor(uint32_t cpu_id, uintptr_t entry)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_SEND_IPI | (entry / 4096));
}

void lapic_enable_spurious()
{
    lapic_write(LAPIC_REG_SPURIOUS, LAPIC_SPURIOUS_ALL | LAPIC_SPURIOUS_ENABLE_APIC);
}

uint32_t apic_get_current_cpu_id(void)
{
    return lapic_read(LAPIC_CPU_ID);
}

uint32_t apic_get_processor_count(void)
{
    return cpu_count;
}

void apic_enable(void)
{
    pic_disable();
    wrmsr(MSR_APIC, rdmsr(MSR_APIC) | LAPIC_ENABLE);
}

struct ioapic_version_reg ioapic_get_version(int ioapic_id)
{
    uint32_t raw = ioapic_read(mmap_phys_to_io(ioapic_table.table[ioapic_id]->address), IOAPIC_REG_VERSION);

    return *((struct ioapic_version_reg *)(&raw));
}
