#pragma once

#include <brutal/base.h>
#include <syscalls/types.h>

BrResult syscall_dispatch(BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5);
