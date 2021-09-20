#include <brutal/log.h>
#include "kernel/cpu.h"
#include "kernel/heap.h"
#include "kernel/kernel.h"
#include "kernel/riscv64/cpu.h"

static size_t impl_count = 0;

static CpuImpl cpus[MAX_CPU_COUNT] = {};

CpuImpl *cpu_impl(CpuId id)
{
    return &cpus[id];
}

CpuImpl *cpu_impl_self(void)
{
    return &cpus[cpu_self_id()];
}

void cpu_found(CpuId id, MAYBE_UNUSED int lapic)
{
    impl_count++;

    cpu(id)->id = id;
    cpu(id)->present = true;
}

/* --- Public API ----------------------------------------------------------- */

int cpu_count(void)
{
    return 1;
}

void cpu_initialize(void)
{
}

CpuId cpu_self_id(void)
{
    return 0;
}

Cpu *cpu_self(void)
{
    return &cpu_impl_self()->base;
}

Cpu *cpu(CpuId id)
{
    return &cpu_impl(id)->base;
}

void cpu_resched_other(MAYBE_UNUSED CpuId cpu)
{
    //
}

void cpu_enable_interrupts(void)
{
    //
}

void cpu_disable_interrupts(void)
{
    //
}
