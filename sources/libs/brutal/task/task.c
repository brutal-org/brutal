#include <brutal/task/task.h>
#include <host/task.h>

static bool task_initialized = false;
static struct task task;

struct task *task_self(void)
{
    if (!task_initialized)
    {
        task.handle = host_task_self();
        task_initialized = true;
    }

    return &task;
}

void task_fork(struct task *task)
{
    *task = (struct task){
        .type = TASK_FORK,
        .fork = {},
    };
}

bool task_is_child(struct task *task)
{
    return task->handle == 0;
}

void task_run(struct task *task)
{
    switch (task->type)
    {
    case TASK_FORK:
        task->handle = host_task_fork();
        break;

    default:
        break;
    }
}

void task_exit(struct task *task, int result)
{
    host_task_exit(task->handle, result);
}

void task_abort(struct task *task)
{
    host_task_abort(task->handle);
}

TaskWaitResult task_wait(struct task *task)
{
    return OK(TaskWaitResult, host_task_wait(task->handle));
}
