#pragma once

#include <brutal/base.h>
#include "kernel/cpu.h"
#include "kernel/x86_64/gdt.h"

#define MAX_CPU_COUNT 255

struct cpu_impl
{
    uintptr_t syscall_kernel_stack;
    uintptr_t syscall_user_stack;

    struct tss tss;

    uint8_t lapic;
    struct cpu base;
};

struct cpu_impl *cpu_impl_self(void);

struct cpu_impl *cpu_impl(CpuId id);

void cpu_found(CpuId id, int lapic);

void cpu_initialize(void); // init cpu_context for current cpu
