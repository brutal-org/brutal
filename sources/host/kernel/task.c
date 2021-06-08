#include <host/task.h>
#include <library/log.h>
task_handle_t host_task_this(void)
{
    todo("add host task this");
    return 0;
}

task_handle_t host_task_fork(void)
{
    todo("add host task fork");
    return 0;
}

int host_task_wait(task_handle_t handle)
{
    todo("add host task wait");
    UNUSED(handle);
    return 0;
}

void host_task_exit(task_handle_t handle, int result)
{
    todo("add host task exit");
    UNUSED(result);
    UNUSED(handle);
}

void host_task_abort(task_handle_t handle)
{
    todo("add host task abort");
    UNUSED(handle);
}