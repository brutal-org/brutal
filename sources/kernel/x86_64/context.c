#include "kernel/context.h"
#include <brutal-alloc>
#include <brutal-debug>
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/gdt.h"
#include "kernel/x86_64/simd.h"
#include "kernel/x86_64/syscall.h"

Context *context_create(void)
{
    Context *self = (Context *)alloc_malloc(alloc_global(), sizeof(Context) + simd_context_size());
    simd_context_init(self->simd);
    return self;
}

void context_destroy(Context *self)
{
    alloc_free(alloc_global(), self);
}

void context_start(Context *self, uintptr_t ip, uintptr_t sp, uintptr_t ksp, BrTaskArgs args, BrTaskFlags flags)
{
    Regs regs = {};

    regs.rip = ip;
    regs.rflags = RFLAGS_INTERRUPT_ENABLE | RFLAGS_RESERVED1_ONE;

    regs.rdi = args.arg1;
    regs.rsi = args.arg2;
    regs.rdx = args.arg3;
    regs.rcx = args.arg4;
    regs.r8 = args.arg5;

    if (flags & BR_TASK_USER)
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

    self->regs = regs;
    self->syscall_kernel_stack = ksp;
}

void context_save(Context *self, Regs const *regs)
{
    simd_context_save(self->simd);
    self->regs = *regs;
}

void context_load(Context *self, Regs *regs)
{
    syscall_set_gs((uintptr_t)self);

    *regs = self->regs;
    simd_context_load(self->simd);
}
