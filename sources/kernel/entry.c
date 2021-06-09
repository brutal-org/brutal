#include <library/base/macros.h>
#include <library/log.h>
#include "arch/arch.h"
#include "kernel/cpu.h"
#include "kernel/kernel.h"
#include "kernel/scheduler.h"
#include "kernel/task.h"

static _Atomic size_t other_ready = 0;

void kernel_splash(void)
{
    log("");
    log("   _____ _____ _____ _____ _____ __      ");
    log("  | __  | __  |  |  |_   _|  _  |  |     ");
    log("  | __ -|    -|  |  | | | |     |  |__   ");
    log("  |_____|__|__|_____| |_| |__|__|_____|  ");
    log("");
    log("-----------------------------------------");
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

    scheduler_initialize();
    tasking_initialize();

    kernel_boot_other();

    log("All CPU started, entering userspace...");

    while (true)
    {
        asm("hlt");
    }
}

void kernel_entry_other(void)
{
    log("CPU N°{} is entering kernel...", cpu_self()->id);

    other_ready++;

    while (true)
    {
        asm("hlt");
    }
}