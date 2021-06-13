#include <brutal/base.h>
#include <brutal/log.h>
#include "arch/arch.h"
#include "arch/cpu.h"
#include "kernel/entry.h"
#include "kernel/tasking.h"

static atomic_int other_ready = 0;

void kernel_splash(void)
{
    log("");
    log("   _____ _____ _____ _____ _____ __      ");
    log("  | __  | __  |  |  |_   _|  _  |  |     ");
    log("  | __ -|    -|  |  | | | |     |  |__   ");
    log("  |_____|__|__|_____| |_| |__|__|_____|  ");
    log("");
    log("-----------------------------------------");
    log("");
}

void kernel_boot_other(void)
{
    arch_boot_other();

    other_ready++;

    while (!(other_ready == cpu_count()))
    {
        task_wait(task_self(), 10);
    }
}

void kernel_entry_main(struct handover *handover)
{
    UNUSED(handover);
    log("Main CPU is entering kernel...");

    kernel_splash();
    tasking_initialize();
    kernel_boot_other();

    log("All CPU started, entering userspace...");

    arch_idle();
}

void kernel_entry_other(void)
{
    log("CPU N°{} is entering kernel...", cpu_self()->id);

    other_ready++;

    arch_idle();
}
