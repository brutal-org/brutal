#include <host/task.h>
#include <library/task/task.h>

static bool task_initialized = false;
static struct task task;

struct task *task_this(void)
{
    if (!task_initialized)
    {
        task.handle = host_task_this();
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

bool task_child(struct task *task)
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

task_wait_result_t task_wait(struct task *task)
{
    return OK(task_wait_result_t, host_task_wait(task->handle));
}