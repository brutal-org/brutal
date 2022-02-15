#pragma once

#include <brutal/base.h>
struct context
{
    uintptr_t syscall_kernel_stack;
    uintptr_t syscall_user_stack;
};
