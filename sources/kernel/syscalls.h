#pragma once

#include <brutal/base.h>
#include <syscalls/types.h>

BrResult syscall_dispatch(BrSyscall syscall, BrArg args);
