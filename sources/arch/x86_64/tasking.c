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

TaskCreateResult arch_task_create(void)
{
    TaskImpl *task = alloc_make(alloc_global(), TaskImpl);

    task->simd_context = (void *)(TRY(TaskCreateResult, heap_alloc(simd_context_size())).base);

    simd_context_init(task->simd_context);

    return OK(TaskCreateResult, (Task *)task);
}

void arch_task_start(Task *task, uintptr_t ip, uintptr_t sp, BrTaskArgs args)
{
    Regs regs;

    regs.rip = ip;
    regs.rflags = RFLAGS_INTERRUPT_ENABLE | RFLAGS_RESERVED1_ONE;

    regs.rdi = args.arg1;
    regs.rsi = args.arg2;
    regs.rdx = args.arg3;
    regs.rcx = args.arg4;
    regs.r8 = args.arg5;

    if (task->flags & TASK_USER)
    {
        regs.cs = (GDT_USER_CODE * 8) | GDT_RING_3;
        regs.ss = (GDT_USER_DATA * 8) | GDT_RING_3;
        regs.rbp = 0;
    }
    else
    {
        regs.cs = (GDT_KERNEL_CODE * 8);
        regs.ss = (GDT_KERNEL_DATA * 8);
        regs.rbp = 0;
    }

    regs.rsp = sp;

    ((TaskImpl *)task)->regs = regs;
}
