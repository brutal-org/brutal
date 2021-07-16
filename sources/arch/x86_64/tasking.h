#pragma once

#include "arch/x86_64/interrupts.h"
#include "kernel/tasking.h"

typedef struct
{
    Task base;
    Regs regs;
    void *simd;
} TaskImpl;
