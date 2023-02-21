#include <bal/abi/syscalls.h>

#if defined(__x86_64__)
BrResult br_syscall(
    BrSyscall syscall,
    BrArg arg1,
    BrArg arg2,
    BrArg arg3,
    BrArg arg4,
    BrArg arg5)
{
    uint64_t res;

    register uint64_t r8 asm("r8") = arg4;
    register uint64_t r9 asm("r9") = arg5;

    asm volatile("syscall"
                 : "=a"(res)
                 : "a"(syscall), "b"(arg1), "d"(arg2), "S"(arg3), "r"(r8), "r"(r9)
                 : "memory", "r11", "rcx");

    return res;
}

#elif defined(__riscv)

BrResult br_syscall(
    [[maybe_unused]] BrSyscall syscall,
    [[maybe_unused]] BrArg arg1,
    [[maybe_unused]] BrArg arg2,
    [[maybe_unused]] BrArg arg3,
    [[maybe_unused]] BrArg arg4,
    [[maybe_unused]] BrArg arg5)
{
    return 0;
}

#else
#    error "Unsupported architecture!"
#endif

BrResult br_log(BrLogArgs *args)
{
    return br_syscall(BR_SC_LOG, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_now(BrNowArgs *args)
{
    return br_syscall(BR_SC_NOW, (BrArg)args, 0, 0, 0, 0);
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

BrResult br_dup(BrDupArgs *args)
{
    return br_syscall(BR_SC_DUP, (BrArg)args, 0, 0, 0, 0);
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

BrResult br_inspect(BrInspectArgs *args)
{
    return br_syscall(BR_SC_INSPECT, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_in(BrIoArgs *args)
{
    return br_syscall(BR_SC_IN, (BrArg)args, 0, 0, 0, 0);
}

BrResult br_out(BrIoArgs *args)
{
    return br_syscall(BR_SC_OUT, (BrArg)args, 0, 0, 0, 0);
}
