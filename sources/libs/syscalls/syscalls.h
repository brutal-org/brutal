#pragma once

#include <syscalls/types.h>

static inline BrResult br_syscall(BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5)
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

static inline BrResult br_noop(void)
{
    return br_syscall(BR_SC_NOOP, 0, 0, 0, 0, 0);
}

static inline BrResult br_log(char const *message, size_t size)
{
    return br_syscall(BR_SC_LOG, (BrArg)message, size, 0, 0, 0);
}

static inline BrResult br_space(BrSpace *space, BrSpaceFlags flags)
{
    return br_syscall(BR_SC_SPACE, (BrArg)space, flags, 0, 0, 0);
}

static inline BrResult br_mobj(BrMObj *mobj, uintptr_t addr, size_t size, BrMObjFlags flags)
{
    return br_syscall(BR_SC_MOBJ, (BrArg)mobj, addr, size, flags, 0);
}

static inline BrResult br_map(BrSpace space, BrMObj mobj, uintptr_t vaddr, BrMemFlags flags)
{
    return br_syscall(BR_SC_MAP, space, mobj, vaddr, flags, 0);
}

static inline BrResult br_alloc(BrSpace space, BrMObj mobj, uintptr_t *vaddr, BrMemFlags flags)
{
    return br_syscall(BR_SC_ALLOC, space, mobj, (BrArg)vaddr, flags, 0);
}

static inline BrResult br_unmap(BrSpace space, uintptr_t vaddr, size_t size)
{
    return br_syscall(BR_SC_UNMAP, space, vaddr, size, 0, 0);
}

static inline BrResult br_task(BrTask *task, BrSpace space, BrTaskFlags flags)
{
    return br_syscall(BR_SC_TASK, (BrArg)task, space, flags, 0, 0);
}

static inline BrResult br_start(BrTask task, uintptr_t ip, uintptr_t sp, BrTaskArgs *args)
{
    return br_syscall(BR_SC_START, task, ip, sp, (BrArg)args, 0);
}

static inline BrResult br_exit(BrTask task, uintptr_t exit_value)
{
    return br_syscall(BR_SC_EXIT, task, exit_value, 0, 0, 0);
}

static inline BrResult br_recv(BrTask task, BrMessage *message, BrTimeout timeout, BrIpcFlags flags)
{
    return br_syscall(BR_SC_RECV, task, (BrArg)message, timeout, flags, 0);
}

static inline BrResult br_irq(BrTask task, BrIrq irq, BrIrqFlags flags)
{
    return br_syscall(BR_SC_IRQ, task, irq, flags, 0, 0);
}

static inline BrResult br_drop(BrTask task, BrCap cap)
{
    return br_syscall(BR_SC_DROP, task, cap, 0, 0, 0);
}

static inline BrResult br_close(BrHandle handle)
{
    return br_syscall(BR_SC_CLOSE, handle, 0, 0, 0, 0);
}
