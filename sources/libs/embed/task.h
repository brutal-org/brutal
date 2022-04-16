#pragma once

#include <brutal-task/task.h>

int embed_task_self(void);

int embed_task_fork(void);

int embed_task_wait(int handle);

void embed_task_exit(int handle, int result);

void embed_task_abort(int handle);
