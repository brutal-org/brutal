#include <library/base.h>
#include <library/log.h>
#include "arch/arch.h"
#include "arch/pmm.h"
#include "arch/vmm.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/com.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/pic.h"
#include "arch/x86_64/pit.h"
#include "arch/x86_64/smid.h"
#include "arch/x86_64/smp.h"
#include "arch/x86_64/stivale2.h"
#include "kernel/kernel.h"

void arch_entry_main(struct handover *handover)
{
    log("Initializing arch x86_64...");

    com_initialize(COM1);
    gdt_initialize();
    idt_initialize();
    simd_initialize();
    pmm_initialize(handover);
    vmm_initialize(handover);

    pic_initialize();
    pic_disable();
    pit_initialize(1000);

    apic_initalize(handover);

    apic_init_interrupt_redirection(handover);

    log("Arch x86_64 initialized!");

    asm_sti();

    kernel_entry_main(handover);
}

void arch_entry_other(void)
{
    apic_enable();
    simd_initialize();
    kernel_entry_other();
}
