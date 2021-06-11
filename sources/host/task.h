#pragma once

#include <brutal/task/task.h>

task_id_t host_task_self(void);

task_id_t host_task_fork(void);

int host_task_wait(task_id_t handle);

void host_task_exit(task_id_t handle, int result);

void host_task_abort(task_id_t handle);
