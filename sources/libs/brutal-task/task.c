#include <embed/task.h>

#include "task.h"

static bool task_initialized = false;
static struct _Task self_task;

struct _Task *task_self(void)
{
    if (!task_initialized)
    {
        self_task.handle = embed_task_self();
        task_initialized = true;
    }

    return &self_task;
}

void task_fork(struct _Task *task)
{
    *task = (struct _Task){
        .type = TASK_FORK,
        .fork = {},
    };
}

bool task_is_child(struct _Task *task)
{
    return task->handle == 0;
}

void task_run(struct _Task *task)
{
    switch (task->type)
    {
    case TASK_FORK:
        task->handle = embed_task_fork();
        break;

    default:
        break;
    }
}

void task_exit(struct _Task *task, int result)
{
    embed_task_exit(task->handle, result);
}

void task_abort(struct _Task *task)
{
    embed_task_abort(task->handle);
}

TaskWaitResult task_wait(struct _Task *task)
{
    return OK(TaskWaitResult, embed_task_wait(task->handle));
}
