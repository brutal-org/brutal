#pragma once

#include <library/task/task.h>

task_handle_t host_task_this(void);

task_handle_t host_task_fork(void);

int host_task_wait(task_handle_t handle);

void host_task_exit(task_handle_t handle, int result);

void host_task_abort(task_handle_t handle);