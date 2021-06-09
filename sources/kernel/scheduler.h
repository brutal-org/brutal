#pragma once

#include <library/base/std.h>

#include "kernel/task.h"

void scheduler_initialize(void);

void scheduler_update(struct task *task, enum task_state old_state, enum task_state new_state);

uintptr_t scheduler_schedule(uintptr_t sp);
