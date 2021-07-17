#pragma once

#include "kernel/x86_64/interrupts.h"

struct context
{
    Regs regs;
    uint8_t simd[] ALIGNED(64);
};
