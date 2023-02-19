#include "kernel/cpu.h"
#include <brutal-debug>
#include "kernel/heap.h"
#include "kernel/kernel.h"
#include "kernel/riscv64/cpu.h"

static CpuImpl _cpus[MAX_CPU_COUNT] = {};
static size_t _len = 0;

CpuImpl *cpu_impl(int id)
{
    return &_cpus[id];
}

CpuImpl *cpu_impl_self(void)
{
    return &_cpus[cpu_self_id()];
}

void cpu_found(int id, MAYBE_UNUSED int lapic)
{
    _len++;

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

int cpu_self_id(void)
{
    return 0;
}

Cpu *cpu_self(void)
{
    return &cpu_impl_self()->base;
}

Cpu *cpu(int id)
{
    return &cpu_impl(id)->base;
}

void cpu_resched_other(MAYBE_UNUSED int cpu)
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
