#pragma once

#include <brutal/base.h>

typedef uint64_t syscall_reg_t;

syscall_reg_t kernel_syscall_handle(syscall_reg_t id, syscall_reg_t arg1, syscall_reg_t arg2, syscall_reg_t arg3, syscall_reg_t arg4, syscall_reg_t arg5);
