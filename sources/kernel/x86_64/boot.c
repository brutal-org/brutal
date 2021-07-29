#include <brutal/base.h>
#include <brutal/log.h>
#include "kernel/arch.h"
#include "kernel/init.h"
#include "kernel/kernel.h"
#include "kernel/pmm.h"
#include "kernel/sched.h"
#include "kernel/tasking.h"
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

static atomic_int other_ready = 0;

void arch_wait_other(void)
{
    WAIT_FOR(other_ready == cpu_count());
}

void arch_boot_other(void)
{
    smp_boot_other();
}

void arch_entry_main(Handover *handover)
{
    cpu_disable_interrupts();
    cpu_retain_disable();

    kernel_splash();

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
    sched_initialize();
    tasking_initialize();

    other_ready++;
    arch_boot_other();
    arch_wait_other();

    cpu_retain_enable();
    cpu_enable_interrupts();

    init_start(handover);

    while (true)
    {
        log("I'm alive!");

        sched_block((Blocker){
            .type = BLOCKER_TIME,
            .deadline = sched_tick() + 1000,
        });
    }

    sched_stop(task_self(), 0);
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

    other_ready++;
    arch_wait_other();

    cpu_retain_enable();
    cpu_enable_interrupts();

    while (true)
    {
        log("I'm alive!");

        sched_block((Blocker){
            .type = BLOCKER_TIME,
            .deadline = sched_tick() + 1000,
        });
    }

    sched_stop(task_self(), 0);
    assert_unreachable();
}
