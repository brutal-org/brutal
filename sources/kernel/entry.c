#include <brutal/base.h>
#include <brutal/log.h>
#include <handover/handover.h>
#include "arch/arch.h"
#include "arch/cpu.h"
#include "kernel/entry.h"
#include "kernel/init.h"
#include "kernel/tasking.h"

static atomic_size_t other_ready = 0;

void kernel_splash(void)
{
    log("BRUTAL - Milestone 1");
    log("\"Long live the UNIX utopia!\"");
}

void kernel_boot_other(void)
{
    arch_boot_other();
    other_ready++;
    WAIT_FOR(other_ready == cpu_count());
}

void kernel_entry_main(struct handover *handover)
{
    kernel_splash();
    tasking_initialize();
    kernel_boot_other();
    arch_enable_interrupt();
    init_start(handover);
    arch_idle();
}

void kernel_entry_other(void)
{
    log("CPU N°{} is entering kernel...", cpu_self()->id);

    other_ready++;
    arch_enable_interrupt();
    arch_idle();
}
