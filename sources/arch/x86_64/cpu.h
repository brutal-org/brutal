#pragma once

#include <library/base/std.h>

#include "arch/cpu.h"
#include "arch/x86_64/apic.h"
#include "kernel/cpu.h"

#define MAX_CPU_COUNT 255

struct x86_64_cpu
{
    void *syscall_user_stack;
    void *syscall_kernel_stack;

    size_t lapic_id;

    struct cpu base;
};

struct x86_64_cpu *x86_64_cpu_this(void);
struct x86_64_cpu *x86_64_cpu(cpuid_t id);