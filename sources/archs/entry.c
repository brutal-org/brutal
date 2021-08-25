
#include <brutal/fibers.h>
#include <brutal/log.h>
#include <syscalls/syscalls.h>

__attribute__((weak)) int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char **argv, MAYBE_UNUSED char **envp)
{
    return 0;
}

__attribute__((weak)) int br_entry(long arg1, long arg2, long arg3, MAYBE_UNUSED long arg4, MAYBE_UNUSED long arg5)
{
    return main(arg1, (char **)arg2, (char **)arg3);
}

void _entry(long arg1, long arg2, long arg3, long arg4, long arg5)
{
    fiber_start(nullptr);

    br_exit(&(BrExitArgs){
        .task = BR_TASK_SELF,
        .exit_value = br_entry(arg1, arg2, arg3, arg4, arg5),
    });
}
