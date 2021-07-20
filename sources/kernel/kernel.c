#include <brutal/base.h>
#include <brutal/log.h>
#include <handover/handover.h>
#include "kernel/arch.h"
#include "kernel/cpu.h"
#include "kernel/init.h"
#include "kernel/kernel.h"
#include "kernel/tasking.h"

static atomic_size_t other_ready = 0;

void kernel_splash(void)
{
    log("BRUTAL - Milestone 2");
    log("\"Long live the UNIX utopia!\"");
}

void kernel_boot_other(void)
{
    arch_boot_other();
    other_ready++;
    WAIT_FOR(other_ready == cpu_count());
}

void kernel_entry_main(Handover *handover)
{
    kernel_splash();
    tasking_initialize();
    kernel_boot_other();

    cpu_retain_enable();
    cpu_enable_interrupts();

    init_start(handover);

    task_cancel(task_self(), 0);
    assert_unreachable();
}

void kernel_entry_other(void)
{
    log("CPU N°{} is entering kernel...", cpu_self()->id);

    other_ready++;

    cpu_retain_enable();
    cpu_enable_interrupts();

    task_cancel(task_self(), 0);
    assert_unreachable();
}
