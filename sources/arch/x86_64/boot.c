#include <brutal/base.h>
#include <brutal/log.h>
#include "arch/arch.h"
#include "arch/pmm.h"
#include "arch/vmm.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/apic/timer.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/com.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/pic.h"
#include "arch/x86_64/pit.h"
#include "arch/x86_64/simd.h"
#include "arch/x86_64/smp.h"
#include "arch/x86_64/stivale2.h"
#include "arch/x86_64/syscall.h"
#include "kernel/entry.h"

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
    pit_initialize(1000);
    apic_initalize(handover);

    cpu_initialize();
    syscall_initialize();


    log("Arch x86_64 initialized!");

    kernel_entry_main(handover);
    assert_unreachable();
}

void arch_entry_other(void)
{
    arch_disable_interrupt();


    apic_enable();
    simd_initialize();

    cpu_initialize();
    syscall_initialize();

    kernel_entry_other();
    assert_unreachable();
}
