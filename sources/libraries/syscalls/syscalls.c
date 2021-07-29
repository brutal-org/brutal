#include <syscalls/syscalls.h>

BrResult br_syscall(BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5)
{
    BrResult result;

    register uint64_t r8 asm("r8") = arg4;
    register uint64_t r9 asm("r9") = arg5;

    asm volatile("syscall"
                 : "=a"(result)
                 : "a"(syscall), "b"(arg1), "d"(arg2), "S"(arg3), "r"(r8), "r"(r9)
                 : "memory", "r11", "rcx");

    return result;
}

BrResult br_log(BrLogArgs *args)
{
    return br_syscall(BR_SC_LOG, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_map(BrMapArgs *args)
{
    return br_syscall(BR_SC_MAP, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_unmap(BrUnmapArgs *args)
{
    return br_syscall(BR_SC_UNMAP, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_create(BrCreateArgs *args)
{
    return br_syscall(BR_SC_CREATE, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_start(BrStartArgs *args)
{
    return br_syscall(BR_SC_START, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_exit(BrExitArgs *args)
{
    return br_syscall(BR_SC_EXIT, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_ipc(BrIpcArgs *args)
{
    return br_syscall(BR_SC_IPC, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_irq(BrIrqArgs *args)
{
    return br_syscall(BR_SC_IRQ, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_drop(BrDropArgs *args)
{
    return br_syscall(BR_SC_DROP, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_close(BrCloseArgs *args)
{
    return br_syscall(BR_SC_CLOSE, (BrArg)args, 0, 0, 0, 0);
}
