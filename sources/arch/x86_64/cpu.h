#pragma once
#include <stddef.h>

#include "arch/x86_64/apic.h"
#include "arch/cpu.h"
#include "kernel/cpu.h"

#define MAX_CPU_COUNT 255

struct x86_64_cpu
{
    void *syscall_stack_save;
    void *syscall_stack;

    size_t lapic_id;

    struct cpu cpu;
    
};

struct x86_64_cpu *x86_64_cpu_this();
struct x86_64_cpu *x86_64_cpu(cpuid_t id);