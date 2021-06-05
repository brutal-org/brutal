#include <library/base.h>
#include <library/log.h>

#include "arch.h"
#include "arch/com.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/pic.h"
#include "arch/pmm.h"
#include "arch/stivale2.h"

void arch_entry(struct handover *handover)
{
    UNUSED(handover);

    log("Initializing arch x86_64...");

    com_initialize(COM1);
    gdt_initialize();
    idt_initialize();
    pic_disable();
    pmm_initialize(&handover->mmap);
}