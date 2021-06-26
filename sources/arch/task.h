#pragma once

#include <brutal/base.h>
#include "kernel/tasking.h"

void arch_task_save_context(struct task *target);

void arch_task_load_context(struct task *target);

task_return_Result arch_task_create(void);

void arch_task_start(
    struct task *task,
    uintptr_t ip,
    uintptr_t arg1,
    uintptr_t arg2,
    uintptr_t arg3,
    uintptr_t arg4,
    uintptr_t arg5);

void arch_task_switch_for_cpu(CpuId cpu);

void arch_task_create_vmm(struct task* target, bool user);
