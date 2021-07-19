#pragma once

#include <brutal/base.h>
#include <brutal/types.h>
#include "kernel/tasking.h"

typedef uint8_t CpuId;

struct cpu
{
    CpuId id;
    bool present;

    bool retain_enable;
    int retain_depth;

    struct schedule schedule;
};

struct cpu *cpu(CpuId id);

size_t cpu_count(void);

struct cpu *cpu_self(void);

CpuId cpu_self_id(void);

void cpu_resched_other(CpuId cpu);

void cpu_begin_interrupt(void);

void cpu_end_interrupt(void);

void cpu_retain_disable(void);

void cpu_retain_enable(void);

void cpu_enable_interrupts(void);

void cpu_disable_interrupts(void);

void cpu_retain_interrupts(void);

void cpu_release_interrupts(void);
