#include "arch/x86_64/syscall.h"
#include <brutal/log.h>
#include "arch/heap.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/interrupts.h"
#include "arch/x86_64/msr.h"
#include "kernel/constants.h"
#include "kernel/syscall.h"

extern void syscall_handle(void);

syscall_initialize_result_t syscall_initialize_for_current_cpu(void)
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

    wrmsr(MSR_LSTAR, (uint64_t)syscall_handle);
    wrmsr(MSR_SYSCALL_FLAG_MASK, SYSCALL_FLAG_TURN_OFF_INTERRUPT);

    todo("cpu -> temporary stack should be in the process [!]");

    uint8_t *cpu_stack = (uint8_t *)TRY(syscall_initialize_result_t, heap_alloc(KERNEL_STACK_SIZE)).base;
    cpu_context_this()->syscall_kernel_stack = cpu_stack + KERNEL_STACK_SIZE;

    return OK(syscall_initialize_result_t, 0);
}

uint64_t arch_syscall_handler(struct interrupt_stackframe *stackframe)
{

    // NOTE: we can't use r11 and rcx because they are used for storing the ip (r11) and the stack (rcx)
    return kernel_syscall_handle(stackframe->rax, stackframe->rbx, stackframe->rdx, stackframe->rsi, stackframe->r8, stackframe->r9);
}