#include <library/base.h>
#include <library/log.h>

#include "arch/arch.h"
#include "arch/pmm.h"

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

    log("Arch x86_64 initialized!");
}