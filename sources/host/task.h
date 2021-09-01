#pragma once

#include <brutal/task/task.h>

TaskId host_task_self(void);

TaskId host_task_fork(void);

int host_task_wait(TaskId handle);

void host_task_exit(TaskId handle, int result);

void host_task_abort(TaskId handle);
