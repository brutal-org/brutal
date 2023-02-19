#include "kernel/x86_64/syscall.h"
#include <brutal-debug>
#include "kernel/heap.h"
#include "kernel/kernel.h"
#include "kernel/syscalls.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/cpu.h"
#include "kernel/x86_64/gdt.h"
#include "kernel/x86_64/interrupts.h"

extern void __syscall(void);

void syscall_initialize(void)
{
    asm_write_msr(MSR_EFER, asm_read_msr(MSR_EFER) | EFER_ENABLE_SYSCALL);

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

    asm_write_msr(MSR_STAR, ((uint64_t)(GDT_KERNEL_CODE * 8) << STAR_KCODE_OFFSET) | ((uint64_t)(((GDT_USER_DATA - 1) * 8) | GDT_RING_3) << STAR_UCODE_OFFSET));
    asm_write_msr(MSR_LSTAR, (uint64_t)__syscall);
    asm_write_msr(MSR_SYSCALL_FLAG_MASK, 0xfffffffe);
}

void syscall_set_gs(uintptr_t addr)
{
    asm_write_msr(MSR_GS_BASE, addr);
    asm_write_msr(MSR_KERN_GS_BASE, addr);
}

uint64_t syscall_handler(Regs const *regs)
{
    // NOTE: we can't use r11 and rcx because they are used for storing the ip (r11) and the flags (rcx)
    return syscall_dispatch((BrSyscall)regs->rax, regs->rbx);
}
