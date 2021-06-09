#include <host/task.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

task_handle_t host_task_self(void)
{
    return getpid();
}

task_handle_t host_task_fork(void)
{
    return fork();
}

int host_task_wait(task_handle_t handle)
{
    int result = -1;
    waitpid(handle, &result, 0);
    return result;
}

void host_task_exit(task_handle_t handle, int result)
{
    if (handle == TASK_THIS)
    {
        _exit(result);
    }
}

void host_task_abort(task_handle_t handle)
{
    if (handle == TASK_THIS)
    {
        _exit(TASK_EXIT_FAILURE);
    }
}