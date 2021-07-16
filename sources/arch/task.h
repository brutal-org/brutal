#pragma once

#include <brutal/base.h>
#include "kernel/tasking.h"

TaskCreateResult arch_task_create(void);

void arch_task_start(Task *task, uintptr_t ip, uintptr_t sp, BrTaskArgs args);
