
#include <brutal/fibers.h>
#include <brutal/log.h>
#include <handover/handover.h>
#include <syscalls/syscalls.h>

__attribute__((weak)) int br_entry_args(
    MAYBE_UNUSED long arg1,
    MAYBE_UNUSED long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{
    panic$("ARGS start protocol is unsupported!");
    return 0;
}

__attribute__((weak)) int main(
    MAYBE_UNUSED int argc,
    MAYBE_UNUSED char **argv,
    MAYBE_UNUSED char **envp)
{
    panic$("CMAIN start protocol is unsupported!");
    return 0;
}

__attribute__((weak)) int br_entry_cmain(
    int argc,
    char **argv,
    char **envp)
{
    return main(argc, argv, envp);
}

__attribute__((weak)) int br_entry_handover(
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
