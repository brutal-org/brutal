#include <brutal/base.h>
#include <brutal/log.h>
#include "kernel/arch.h"
#include "kernel/kernel.h"
#include "kernel/pmm.h"
#include "kernel/vmm.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/apic/timer.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/com.h"
#include "kernel/x86_64/cpu.h"
#include "kernel/x86_64/gdt.h"
#include "kernel/x86_64/hpet.h"
#include "kernel/x86_64/idt.h"
#include "kernel/x86_64/pic.h"
#include "kernel/x86_64/simd.h"
#include "kernel/x86_64/smp.h"
#include "kernel/x86_64/stivale2.h"
#include "kernel/x86_64/syscall.h"

void arch_entry_main(Handover *handover)
{
    cpu_disable_interrupts();
    cpu_retain_disable();

    log("Initializing arch x86_64...");

    com_initialize(COM1);
    gdt_initialize();
    idt_initialize();
    simd_initialize();
    pmm_initialize(handover);
    vmm_initialize(handover);
    pic_initialize();
    hpet_initialize(handover);
    apic_initalize(handover);
    cpu_initialize();
    syscall_initialize();

    log("Arch x86_64 initialized!");

    kernel_entry_main(handover);
    assert_unreachable();
}

void arch_entry_other(void)
{
    cpu_disable_interrupts();
    cpu_retain_disable();

    apic_enable();
    simd_initialize();

    cpu_initialize();
    syscall_initialize();

    kernel_entry_other();
    assert_unreachable();
}
