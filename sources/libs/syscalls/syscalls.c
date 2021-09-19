#include <syscalls/syscalls.h>
#include <host/asm.h>

BrResult br_syscall(BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5)
{
    return (BrResult)asm_syscall(syscall, arg1, arg2, arg3, arg4, arg5);
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

BrResult br_drop(BrDropArgs *args)
{
    return br_syscall(BR_SC_DROP, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_close(BrCloseArgs *args)
{
    return br_syscall(BR_SC_CLOSE, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_bind(BrBindArgs *args)
{
    return br_syscall(BR_SC_BIND, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_unbind(BrUnbindArgs *args)
{
    return br_syscall(BR_SC_UNBIND, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_ack(BrAckArgs *args)
{
    return br_syscall(BR_SC_ACK, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_stat(BrStatArgs *args)
{
    return br_syscall(BR_SC_STAT, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_in(BrIoArgs *args)
{
    return br_syscall(BR_SC_IN, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_out(BrIoArgs *args)
{
    return br_syscall(BR_SC_OUT, (BrArg)args, 0, 0, 0, 0);
}
