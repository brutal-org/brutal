#pragma once

#include <brutal/io.h>
#include <handover/handover.h>
#include "kernel/tasking.h"

IoWriter *arch_debug(void);

void arch_entry_main(struct handover *handover);

void arch_entry_other(void);

void arch_boot_other(void);

void arch_resched_other(void);

void arch_idle(void);

void arch_stop(void);

Task *arch_create_task(void);

void arch_destroy_task(Task *task);

void arch_enable_interrupt(void);

void arch_disable_interrupt(void);

void arch_cpu_pause(void);

void arch_cpu_halt(void);
