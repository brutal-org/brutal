#pragma once

#include "kernel/tasking.h"
#include <brutal/base.h>

void arch_task_save_context(struct task * target);

void arch_task_load_context(struct task * target);

task_return_result_t arch_task_create(uintptr_t ip, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4);
