#pragma once

#include <brutal/task/task.h>

TaskId embed_task_self(void);

TaskId embed_task_fork(void);

int embed_task_wait(TaskId handle);

void embed_task_exit(TaskId handle, int result);

void embed_task_abort(TaskId handle);
