#include <brutal/log.h>
#include "kernel/syscall.h"

syscall_reg_t kernel_syscall_handle(
    syscall_reg_t id,
    syscall_reg_t arg1,
    syscall_reg_t arg2,
    syscall_reg_t arg3,
    syscall_reg_t arg4,
    syscall_reg_t arg5)
{
    log("called syscall: {p} arg: {p} {p} {p} {p} {p}", id, arg1, arg2, arg3, arg4, arg5);
    return 0;
}
