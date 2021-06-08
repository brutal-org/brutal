#include <library/base.h>
#include <library/log.h>

#include "arch/arch.h"
#include "arch/pmm.h"
#include "arch/vmm.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/com.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/smid.h"
#include "arch/x86_64/stivale2.h"
#include "kernel/kernel.h"

void arch_entry(struct handover *handover)
{
    log("Initializing arch x86_64...");

    com_initialize(COM1);
    gdt_initialize();
    idt_initialize();
    simd_initialize();
    pmm_initialize(handover);
    vmm_initialize(handover);
    apic_initalize(handover);

    log("Arch x86_64 initialized!");

    kernel_entry(handover);
}