#include <library/base.h>
#include <library/log.h>

#include "arch/arch.h"
#include "arch/pmm.h"

#include "arch/x86_64/acpi.h"
#include "arch/x86_64/com.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/pic.h"
#include "arch/x86_64/stivale2.h"

void arch_entry(struct handover *handover)
{
    UNUSED(handover);

    log("Initializing arch x86_64...");

    com_initialize(COM1);
    gdt_initialize();
    idt_initialize();
    pic_disable();
    pmm_initialize(handover);

    auto lapic = acpi_find_lapic(handover->rsdp);
    auto ioapic = acpi_find_ioapic(handover->rsdp);

    log("Lapic: {x}", lapic);
    log("Iopic: {x}", ioapic);

    log("Arch x86_64 initialized!");
}