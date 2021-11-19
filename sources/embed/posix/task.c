#include <embed/task.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

TaskId host_task_self(void)
{
    return getpid();
}

TaskId host_task_fork(void)
{
    return fork();
}

int host_task_wait(TaskId handle)
{
    int result = -1;
    waitpid(handle, &result, 0);
    return result;
}

void host_task_exit(TaskId handle, int result)
{
    if (handle == TASK_THIS)
    {
        exit(result);
    }
}

void host_task_abort(TaskId handle)
{
    if (handle == TASK_THIS)
    {
        exit(TASK_EXIT_FAILURE);
    }
}
