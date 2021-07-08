#pragma once

#include <brutal/io.h>
#include "kernel/handover.h"

IoWriter *arch_debug(void);

void arch_entry_main(struct handover *handover);

void arch_entry_other(void);

void arch_boot_other(void);

void arch_resched_other(void);

void arch_idle(void);

void arch_stop(void);

struct task *arch_create_task(void);

void arch_save_task_context(struct task *task);

void arch_restore_task_context(struct task *task);

void arch_destroy_task(struct task *task);

void arch_enable_interrupt();

void arch_disable_interrupt();

void arch_cpu_pause();

void arch_cpu_halt();
