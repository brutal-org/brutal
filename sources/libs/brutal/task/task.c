#include <brutal/task/task.h>
#include <embed/task.h>

static bool task_initialized = false;
static struct task self_task;

struct task *task_self(void)
{
    if (!task_initialized)
    {
        self_task.handle = embed_task_self();
        task_initialized = true;
    }

    return &self_task;
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
        task->handle = embed_task_fork();
        break;

    default:
        break;
    }
}

void task_exit(struct task *task, int result)
{
    embed_task_exit(task->handle, result);
}

void task_abort(struct task *task)
{
    embed_task_abort(task->handle);
}

TaskWaitResult task_wait(struct task *task)
{
    return OK(TaskWaitResult, embed_task_wait(task->handle));
}
