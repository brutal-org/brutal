#pragma once

#include "kernel/cpu.h"

#define MAX_CPU_COUNT 255

typedef struct
{
    Cpu base;
} CpuImpl;

CpuImpl *cpu_impl_self(void);

CpuImpl *cpu_impl(int id);

void cpu_found(int id, int lapic);

void cpu_initialize(void); // init cpu_context for current cpu
