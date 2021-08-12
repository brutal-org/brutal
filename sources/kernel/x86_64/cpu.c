#include <brutal/log.h>
#include "kernel/cpu.h"
#include "kernel/heap.h"
#include "kernel/kernel.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/cpu.h"

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

void cpu_found(CpuId id, int lapic)
{
    impl_count++;

    cpu(id)->id = id;
    cpu(id)->present = true;

    cpu_impl(id)->lapic = lapic;

    // Irq and Isr stack
    cpu_impl(id)->tss.rsp[0] = range_end(UNWRAP(heap_alloc(KERNEL_STACK_SIZE)));

    // Shed stack
    cpu_impl(id)->tss.ist[0] = range_end(UNWRAP(heap_alloc(KERNEL_STACK_SIZE)));
    cpu_impl(id)->tss.ist[1] = range_end(UNWRAP(heap_alloc(KERNEL_STACK_SIZE)));
}

/* --- Public API ----------------------------------------------------------- */

int cpu_count(void)
{
    return impl_count;
}

void cpu_initialize(void)
{
    // Setup swapgs
    asm_write_msr(MSR_GS_BASE, (uintptr_t)cpu_impl_self());
    asm_write_msr(MSR_KERN_GS_BASE, (uintptr_t)cpu_impl_self());

    // Setup tss
    gdt_load_tss(&cpu_impl_self()->tss);
}

CpuId cpu_self_id(void)
{
    return lapic_current_cpu();
}

Cpu *cpu_self(void)
{
    return &cpu_impl_self()->base;
}

Cpu *cpu(CpuId id)
{
    return &cpu_impl(id)->base;
}

void cpu_resched_other(CpuId cpu)
{
    lapic_send_ipi(cpu, IPI_RESCHED);
}

void cpu_enable_interrupts(void)
{
    asm_sti();
}

void cpu_disable_interrupts(void)
{
    asm_cli();
}
