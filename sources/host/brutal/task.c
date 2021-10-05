#include <bal/helpers.h>
#include <bal/syscalls.h>
#include <host/task.h>

TaskId host_task_self(void)
{
    //FIXME: return the real tid
    return (TaskId)BR_TASK_SELF;
}

void host_task_exit(TaskId handle, int result)
{
    br_exit(&(BrExitArgs){
        .task = handle,
        .exit_value = result,
    });
}

void host_task_abort(TaskId handle)
{
    br_exit(&(BrExitArgs){
        .task = handle,
        .exit_value = TASK_EXIT_FAILURE,
    });
}
