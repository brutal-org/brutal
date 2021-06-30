#include "arch/cpu.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/msr.h"

static size_t impl_count = 0;
static struct cpu_impl impls[MAX_CPU_COUNT];

struct cpu_impl *cpu_impl(CpuId id)
{
    return &impls[id];
}

struct cpu_impl *cpu_impl_self(void)
{
    return &impls[cpu_self_id()];
}

void cpu_found(CpuId id, int lapic)
{
    impl_count++;

    cpu(id)->id = id;
    cpu(id)->present = true;
    cpu_impl(id)->lapic = lapic;
}

/* --- Public API ----------------------------------------------------------- */

size_t cpu_count(void)
{
    return impl_count;
}

void cpu_initialize(void)
{
    wrmsr(MSR_GS_BASE, (uintptr_t)cpu_impl_self());
    wrmsr(MSR_KERN_GS_BASE, (uintptr_t)cpu_impl_self());
}
/* --- Public API ----------------------------------------------------------- */

CpuId cpu_self_id(void)
{
    return apic_current_cpu();
}

struct cpu *cpu_self(void)
{
    return &cpu_impl_self()->base;
}

struct cpu *cpu(CpuId id)
{
    return &cpu_impl(id)->base;
}
