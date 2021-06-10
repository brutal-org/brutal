#include "arch/x86_64/tasking.h"
#include <brutal/alloc.h>
#include <brutal/base.h>
#include "arch/heap.h"
#include "arch/task.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/interrupts.h"
#include "arch/x86_64/simd.h"
#include "kernel/constants.h"

void arch_task_save_context(struct task *target)
{
    struct arch_task *task = (struct arch_task *)target;
    simd_context_save(task->simd_context);
}

void arch_task_load_context(struct task *target)
{
    struct arch_task *task = (struct arch_task *)target;
    simd_context_load(task->simd_context);
}

task_return_result_t arch_task_create(uintptr_t ip, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4)
{
    struct arch_task *task = alloc_make(alloc_global(), struct arch_task);

    task->base.ip = ip;
    task->base.kernel_sp = TRY(task_return_result_t, heap_alloc(KERNEL_STACK_SIZE)).base + KERNEL_STACK_SIZE;
    task->base.sp = TRY(task_return_result_t, heap_alloc(KERNEL_STACK_SIZE)).base + KERNEL_STACK_SIZE;

    task->base.sp -= sizeof(struct interrupt_stackframe);

    struct interrupt_stackframe *stackframe = (struct interrupt_stackframe *)task->base.sp;

    stackframe->rip = ip;
    stackframe->rdi = arg1;
    stackframe->rsi = arg2;
    stackframe->rdx = arg3;
    stackframe->rcx = arg4;
    stackframe->cs = (GDT_USER_CODE * 8) | GDT_RING_3;
    stackframe->ss = (GDT_USER_DATA * 8) | GDT_RING_3;
    stackframe->rsp = task->base.sp + sizeof(struct interrupt_stackframe);

    return OK(task_return_result_t, (struct task *)task);
}
