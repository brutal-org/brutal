#include <brutal/base.h>
#include <brutal/log.h>
#include "arch/arch.h"
#include "arch/cpu.h"
#include "kernel/entry.h"
#include "kernel/tasking.h"

static atomic_size_t other_ready = 0;

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
    while (other_ready != cpu_count())
    {
    }
}

void task_test(void)
{
    while (true)
    {
        log("task {} {}", task_self()->id, cpu_self_id());
        for (size_t i = 0; i < 10000; i++)
        {
            arch_cpu_pause();
        }
    }
}

void kernel_entry_main(MAYBE_UNUSED struct handover *handover)
{
    kernel_splash();

    tasking_initialize();
    log("Main CPU is entering kernel...");

    for (size_t i = 0; i < 20; i++)
    {
        auto test_task = UNWRAP(task_create(make_str("test-task"), TASK_NONE));
        task_start(test_task, (uintptr_t)task_test, 0, 0, 0, 0, 0);
    }

    kernel_boot_other();

    log("All CPU started, entering userspace...");
    arch_enable_interrupt();
    arch_idle();
}

void kernel_entry_other(void)
{
    log("CPU N°{} is entering kernel...", cpu_self()->id);

    other_ready++;
    arch_enable_interrupt();
    arch_idle();
}
