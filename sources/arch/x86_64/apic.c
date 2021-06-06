#include <library/base/std.h>
#include <library/log.h>

#include "arch/x86_64/acpi.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/mmap.h"
#include "arch/x86_64/pic.h"
#include "kernel/mmio.h"

static uintptr_t lapic_base;
static uintptr_t ioapic_base;

void apic_initalize(struct handover const *handover)
{
    lapic_base = mmap_phys_to_io(acpi_find_lapic(handover->rsdp));
    ioapic_base = mmap_phys_to_io(acpi_find_ioapic(handover->rsdp));

    pic_disable();

    log("Lapic found at {p}", lapic_base);
    log("Ioapic found at {p}", ioapic_base);
}

void apic_eoi(void)
{
    mmio_write32(lapic_base + LAPIC_REG_EOI, 0);
}
