#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/log.h>
#include "arch/heap.h"
#include "arch/task.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/interrupts.h"
#include "arch/x86_64/simd.h"
#include "arch/x86_64/syscall.h"
#include "arch/x86_64/tasking.h"
#include "kernel/constants.h"

void arch_task_create_vmm(struct task *target, bool user)
{
    if (user)
    {
        target->virtual_memory_space = vmm_space_create();
    }
    else
    {
        target->virtual_memory_space = vmm_kernel_space();
    }
}

TaskCreateResult arch_task_create(void)
{
    TaskImpl *task = alloc_make(alloc_global(), TaskImpl);

    task->simd_context = (void *)(TRY(TaskCreateResult, heap_alloc(simd_context_size())).base);

    simd_context_init(task->simd_context);

    return OK(TaskCreateResult, (struct task *)task);
}

void arch_task_start(struct task *task, uintptr_t ip, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4, uintptr_t arg5)
{
    struct interrupt_stackframe regs;

    regs.rip = ip;
    regs.rflags = RFLAGS_INTERRUPT_ENABLE | RFLAGS_RESERVED1_ONE;
    regs.rdi = arg1;
    regs.rsi = arg2;
    regs.rdx = arg3;
    regs.rcx = arg4;
    regs.rdx = arg5;

    if (task->flags & TASK_USER)
    {
        regs.cs = (GDT_USER_CODE * 8) | GDT_RING_3;
        regs.ss = (GDT_USER_DATA * 8) | GDT_RING_3;
        regs.rsp = USER_STACK_BASE;
    }
    else
    {
        regs.cs = (GDT_KERNEL_CODE * 8);
        regs.ss = (GDT_KERNEL_DATA * 8);
        regs.rsp = task->ksp;
    }

    ((TaskImpl *)task)->regs = regs;
}
