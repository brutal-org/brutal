#include "arch/x86_64/cpu.h"

static struct x86_64_cpu cpu_array[MAX_CPU_COUNT];

struct x86_64_cpu *x86_64_cpu_this(void)
{
    return &cpu_array[arch_cpu_current_id()];
}

struct x86_64_cpu *x86_64_cpu(cpuid_t id)
{
    return &cpu_array[id];
}

size_t arch_cpu_count(void)
{
    return apic_get_processor_count();
}

cpuid_t arch_cpu_current_id(void)
{
    return apic_get_current_cpu_id();
}

struct cpu *arch_cpu_this(void)
{
    return &x86_64_cpu_this()->base;
}

struct cpu *arch_cpu(cpuid_t id)
{
    return &x86_64_cpu(id)->base;
}