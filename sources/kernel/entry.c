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
        if (task_self() == nullptr)
        {
            log("task WTF", task_self()->id);
        }

        log("task {} {}", task_self()->id, cpu_self_id());

        for (size_t i = 0; i < 10000; i++)
        {
            asm volatile("pause");
        }
    }
}

void kernel_entry_main(struct handover *handover)
{
    UNUSED(handover);
    kernel_splash();

    tasking_initialize();
    log("Main CPU is entering kernel...");

    for (size_t i = 0; i < 20; i++)
    {
        task_create((uintptr_t)task_test, TASK_CREATE_START_DIRECT);
    }

    log("All CPU started, entering userspace...");
    kernel_boot_other();

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
