#include <bal/abi.h>
#include <bal/boot.h>
#include <ipc/ipc.h>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-fibers>

WEAK int main(int, char **)
{
    panic$("No ipc_component_main() defined");
}

WEAK int ipc_component_main(MAYBE_UNUSED IpcComponent *self)
{
    return main(0, nullptr);
}

WEAK int br_main(
    long arg1,
    long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{
    IpcComponent component;
    ipc_component_init(&component, alloc_global());
    ipc_component_inject(&component, (IpcCap *)arg1, (int)arg2);
    int result = ipc_component_main(&component);
    ipc_component_deinit(&component);
    return result;
}

void _entry(long arg1, long arg2, long arg3, long arg4, long arg5)
{
    fiber_start(nullptr, nullptr);

    int result = br_main(arg1, arg2, arg3, arg4, arg5);

    br_exit(&(BrExitArgs){
        .handle = BR_HANDLE_SELF,
        .result = result,
    });
}
