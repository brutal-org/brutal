#pragma once

#include "kernel/x86_64/interrupts.h"

struct context
{
    uintptr_t syscall_kernel_stack;
    uintptr_t syscall_user_stack;

    Regs regs;
    uint8_t simd[] ALIGNED(64);
};

void context_save(struct context *self, Regs const *regs);

void context_load(struct context *self, Regs *regs);
