#pragma once

#include <syscalls/types.h>

#ifdef __kernel__
#    define SYSCALL(NAME) BrResult sys_##NAME
#else
#    define SYSCALL(NAME) BrResult br_##NAME
SYSCALL(syscall)
(BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5);
#endif

// clang-format off

SYSCALL(noop) (void);

SYSCALL(log) (char const *message, size_t size);

SYSCALL(space) (BrSpace *space, BrSpaceFlags flags);

SYSCALL(mobj) (BrMObj *mobj, uintptr_t addr, size_t size, BrMObjFlags flags);

SYSCALL(map) (BrSpace space, BrMObj mobj, uintptr_t vaddr, BrMemFlags flags);

SYSCALL(alloc) (BrSpace space, BrMObj mobj, uintptr_t *vaddr, BrMemFlags flags);

SYSCALL(unmap) (BrSpace space, uintptr_t vaddr, size_t size);

SYSCALL(task) (BrTask *task, BrSpace space, BrTaskFlags flags);

SYSCALL(start) (BrTask task, uintptr_t ip, uintptr_t sp, BrTaskArgs *args);

SYSCALL(exit) (BrTask task, uintptr_t exit_value);

SYSCALL(send) (BrTask task, BrMessage const *message, BrTimeout timeout, BrIpcFlags flags);

SYSCALL(recv) (BrTask task, BrMessage *message, BrTimeout timeout, BrIpcFlags flags);

SYSCALL(irq) (BrTask task, BrIrq irq, BrIrqFlags flags);

SYSCALL(drop) (BrTask task, BrCap cap);

SYSCALL(close) (BrHandle handle);

// clang-format on
