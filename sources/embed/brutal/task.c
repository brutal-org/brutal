#include <bal/abi.h>
#include <embed/task.h>

TaskId embed_task_self(void)
{
    // FIXME: return the real tid
    return (TaskId)BR_TASK_SELF;
}

void embed_task_exit(TaskId handle, int result)
{
    br_exit(&(BrExitArgs){
        .task = handle,
        .exit_value = result,
    });
}

void embed_task_abort(TaskId handle)
{
    br_exit(&(BrExitArgs){
        .task = handle,
        .exit_value = TASK_EXIT_FAILURE,
    });
}
