#pragma once

#include "kernel/cpu.h"
#include "kernel/x86_64/gdt.h"

#define MAX_CPU_COUNT 255

typedef struct
{
    Tss tss;
    uint8_t lapic;
    Cpu base;
} CpuImpl;

CpuImpl *cpu_impl_self(void);

CpuImpl *cpu_impl(CpuId id);

void cpu_found(CpuId id, int lapic);

void cpu_initialize(void); // init cpu_context for current cpu
