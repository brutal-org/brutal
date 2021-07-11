#pragma once

#include "arch/x86_64/interrupts.h"
#include "kernel/tasking.h"

typedef struct
{
    struct task base;
    struct interrupt_stackframe regs;

    void *simd_context;
} TaskImpl;
