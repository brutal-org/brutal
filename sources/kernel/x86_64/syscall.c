#include "kernel/x86_64/syscall.h"
#include <brutal/log.h>
#include "kernel/heap.h"
#include "kernel/kernel.h"
#include "kernel/syscalls.h"
#include "kernel/x86_64/cpu.h"
#include "kernel/x86_64/gdt.h"
#include "kernel/x86_64/interrupts.h"
#include "kernel/x86_64/msr.h"

extern void __syscall(void);

void syscall_initialize(void)
{
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | EFER_ENABLE_SYSCALL);

    // when starting the syscall:
    // CS = kcode
    // SS = kcode + 8
    // when returning:
    // cs=  ucode + 16
    // ss = ucode + 8

    // so we need to have:
    // kcode : kernel code
    // kcode + 8: kernel data
    // ucode + 8 : user data
    // ucode + 16 : user code

    wrmsr(MSR_STAR, ((uint64_t)(GDT_KERNEL_CODE * 8) << STAR_KCODE_OFFSET) |
                        ((uint64_t)(((GDT_USER_DATA - 1) * 8) | GDT_RING_3) << STAR_UCODE_OFFSET));

    wrmsr(MSR_LSTAR, (uint64_t)__syscall);
    wrmsr(MSR_SYSCALL_FLAG_MASK, 0);
}

void syscall_set_stack(uintptr_t stack)
{
    cpu_impl_self()->syscall_kernel_stack = stack;
}

uint64_t syscall_handler(Regs *regs)
{
    // NOTE: we can't use r11 and rcx because they are used for storing the ip (r11) and the stack (rcx)
    return syscall_dispatch(regs->rax, regs->rbx, regs->rdx, regs->rsi, regs->r8, regs->r9);
}
