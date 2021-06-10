#pragma once

#include "kernel/tasking.h"

struct arch_task
{
    struct task base;

    void *simd_context;
};
