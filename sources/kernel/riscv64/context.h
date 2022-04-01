#pragma once

#include <bruwutal/base.h>
struct _Context
{
    uintptr_t syscall_kernel_stack;
    uintptr_t syscall_user_stack;
};
