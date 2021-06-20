#pragma once

#include <brutal/base.h>
#include "arch/cpu.h"

#define MAX_CPU_COUNT 255

struct cpu_impl
{
    void *syscall_kernel_stack;
    void *syscall_user_stack;

    uint8_t lapic;
    struct cpu base;
};

struct cpu_impl *cpu_impl_self(void);

struct cpu_impl *cpu_impl(cpu_id_t id);

void cpu_found(cpu_id_t id, int lapic);

void cpu_initialize(void); // init cpu_context for current cpu
