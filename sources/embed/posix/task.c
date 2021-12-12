#include <embed/task.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

TaskId embed_task_self(void)
{
    return getpid();
}

TaskId embed_task_fork(void)
{
    return fork();
}

int embed_task_wait(TaskId handle)
{
    int result = -1;
    waitpid(handle, &result, 0);
    return result;
}

void embed_task_exit(TaskId handle, int result)
{
    if (handle == TASK_THIS)
    {
        exit(result);
    }
}

void embed_task_abort(TaskId handle)
{
    if (handle == TASK_THIS)
    {
        exit(TASK_EXIT_FAILURE);
    }
}
