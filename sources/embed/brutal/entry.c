#include <bal/abi.h>
#include <bal/boot.h>
#include <brutal/debug.h>
#include <brutal/fibers.h>

WEAK int br_entry_args(
    MAYBE_UNUSED long arg1,
    MAYBE_UNUSED long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{
    panic$("ARGS start protocol is unsupported!");
    return 0;
}

WEAK int main(
    MAYBE_UNUSED int argc,
    MAYBE_UNUSED char **argv,
    MAYBE_UNUSED char **envp)
{
    panic$("CMAIN start protocol is unsupported!");
    return 0;
}

WEAK int br_entry_cmain(
    int argc,
    char **argv,
    char **envp)
{
    return main(argc, argv, envp);
}

WEAK int br_entry_handover(
    MAYBE_UNUSED Handover *handover)
{
    panic$("HANDOVER start protocol is unsupported!");
    return 0;
}

void _entry(BrStartType type, long arg1, long arg2, long arg3, long arg4, long arg5)
{
    fiber_start(nullptr);

    int exit_value = -1;

    switch (type)
    {
    case BR_START_ARGS:
        exit_value = br_entry_args(arg1, arg2, arg3, arg4, arg5);
        break;

    case BR_START_CMAIN:
        exit_value = br_entry_cmain((int)arg1, (char **)arg2, (char **)arg3);
        break;

    case BR_START_HANDOVER:
        exit_value = br_entry_handover((Handover *)arg1);
        break;

    default:
        panic$("Unknown startup protocol!");
    }

    br_exit(&(BrExitArgs){
        .task = BR_TASK_SELF,
        .exit_value = exit_value,
    });
}
