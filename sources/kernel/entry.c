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

    arch_idle();
}

void task1(void)
{
    while (true)
    {
        log("task 1");
    }
}
void task2(void)
{
    while (true)
    {
        log("task 2");
    }
}
void task3(void)
{
    while (true)
    {
        log("task 3");
    }
}
void task4(void)
{
    while (true)
    {
        log("task 4");
    }
}
void task5(void)
{
    while (true)
    {
        log("task 5");
    }
}
void task6(void)
{
    while (true)
    {
        log("task 6");
    }
}
void task7(void)
{
    while (true)
    {
        log("task 7");
    }
}
void task8(void)
{
    while (true)
    {
        log("task 8");
    }
}

void kernel_entry_main(struct handover *handover)
{
    UNUSED(handover);
    log("Main CPU is entering kernel...");

    kernel_splash();
    tasking_initialize();
    task_spawn((uintptr_t)task1, true);
    task_spawn((uintptr_t)task2, true);
    task_spawn((uintptr_t)task3, true);
    task_spawn((uintptr_t)task4, true);
    task_spawn((uintptr_t)task5, true);
    task_spawn((uintptr_t)task6, true);
    task_spawn((uintptr_t)task7, true);
    task_spawn((uintptr_t)task8, true);
    arch_enable_interrupt();
    log("All CPU started, entering userspace...");
    kernel_boot_other();
}

void kernel_entry_other(void)
{
    log("CPU N°{} is entering kernel...", cpu_self()->id);

    other_ready++;
    arch_enable_interrupt();
    arch_idle();
}
