#include <bal/abi.h>
#include <embed/task.h>
#include <brutal-debug>

int embed_task_self(void)
{
    BrInspectArgs inspect = {
        .handle = BR_HANDLE_SELF,
    };

    assert_br_success(br_inspect(&inspect));
    return (int)inspect.id;
}

void embed_task_exit(int handle, int result)
{
    br_exit(&(BrExitArgs){
        .handle = handle,
        .result = result,
    });
}

void embed_task_abort(int handle)
{
    br_exit(&(BrExitArgs){
        .handle = handle,
        .result = TASK_EXIT_FAILURE,
    });
}
