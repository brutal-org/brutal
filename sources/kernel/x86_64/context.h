#pragma once

#include "kernel/x86_64/interrupts.h"

struct _Context
{
    uintptr_t syscall_kernel_stack;
    uintptr_t syscall_user_stack;

    Regs regs;
    uint8_t simd[] ALIGNED(64);
};

void context_save(struct _Context *self, Regs const *regs);

void context_load(struct _Context *self, Regs *regs);
