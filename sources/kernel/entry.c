#include <brutal/base.h>
#include <brutal/log.h>
#include <handover/handover.h>
#include "arch/arch.h"
#include "arch/cpu.h"
#include "kernel/entry.h"
#include "kernel/loader.h"
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

void kernel_entry_main(MAYBE_UNUSED struct handover *handover)
{
    kernel_splash();
    tasking_initialize();
    kernel_boot_other();
    arch_enable_interrupt();

    log("All CPU started, entering userspace...");

    struct handover_module *mod = handover_find_module(handover, str_cast("bootstrap"));

    uintptr_t start = 0;
    auto my_task = program_load(str_cast("bootstrap"), (void *)mod->addr, mod->size, &start);
    task_start(UNWRAP(my_task), start, 0, 0, 0, 0, 0);

    arch_idle();
}

void kernel_entry_other(void)
{
    log("CPU N°{} is entering kernel...", cpu_self()->id);

    other_ready++;
    arch_enable_interrupt();
    arch_idle();
}
