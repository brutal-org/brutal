#pragma once

#include <library/base/std.h>

#define MAX_CPU_COUNT 255

struct cpu_context
{
    void *syscall_user_stack;
    void *syscall_kernel_stack;

    uint8_t lapic;
    struct cpu base;
};

struct cpu_context *cpu_context_this(void);

struct cpu_context *cpu_context(cpu_id_t id);

void cpu_context_found(cpu_id_t id, int lapic);
