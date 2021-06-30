#pragma once

#include <brutal/base.h>
#include <brutal/types.h>
#include "kernel/tasking.h"

typedef uint8_t CpuId;

struct cpu
{
    CpuId id;
    bool present;

    struct schedule schedule;
};

struct cpu *cpu(CpuId id);

size_t cpu_count(void);

struct cpu *cpu_self(void);

CpuId cpu_self_id(void);
