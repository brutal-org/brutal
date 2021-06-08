#include "arch/x86_64/smp.h"
#include <library/mem.h>

#include "arch/pmm.h"
#include "arch/vmm.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/mmap.h"
#include "host/mem.h"
#include "kernel/mmio.h"

typedef result_t(br_error_t, uint8_t) smp_init_result;
extern uint32_t trampoline_start, trampoline_end, nstack;
bool cpu_started = false;
void smp_started_cpu_start()
{
    apic_enable();
    cpu_context_this()->base.present = true;

    cpu_started = false;
    cpu_start(cpu_context_this()->base.id);
}

smp_init_result smp_initialize_cpu_trampoline()
{
    uint64_t trampoline_len = (uintptr_t)&trampoline_end - (uintptr_t)&trampoline_start;

    vmm_map(vmm_kernel_space(),
            (vmm_range_t){
                .base = (0x0),
                .size = ALIGN_UP(trampoline_len, HOST_MEM_PAGESIZE) + HOST_MEM_PAGESIZE},
            (pmm_range_t){
                .base = (0x0),
                .size = ALIGN_UP(trampoline_len, HOST_MEM_PAGESIZE) + HOST_MEM_PAGESIZE},
            BR_MEM_WRITABLE);

    vmm_space_switch(vmm_kernel_space());

    mem_cpy((void *)0x1000, (void *)&trampoline_start, trampoline_len);

    return OK(smp_init_result, 0);
}

smp_init_result smp_initialize_cpu_future_value()
{
    mmio_write64(SMP_INIT_PAGE_TABLE, asm_read_cr3());

    uint8_t *cpu_stack = (uint8_t *)mmap_phys_to_io(TRY(smp_init_result, pmm_alloc(SMP_CPU_STACK_SIZE)).base);

    mmio_write64(SMP_INIT_STACK_ADDR, (uint64_t)(cpu_stack + SMP_CPU_STACK_SIZE));

    // gdt at 0x580
    // idt at 0x590
    asm volatile(" \n"
                 "sgdt 0x580\n"
                 "sidt 0x590\n");

    mmio_write64(SMP_INIT_START_ADDR, (uintptr_t)&smp_started_cpu_start);
    return OK(smp_init_result, 0);
}

void smp_initialize_cpu(cpu_id_t cpu, uint32_t lapic_id)
{
    UNUSED(cpu);
    apic_init_processor(lapic_id);

    smp_initialize_cpu_trampoline();
    smp_initialize_cpu_future_value();

    apic_start_processor(lapic_id, 4096);
}

void wait_for_cpu()
{
    while (cpu_started)
    {
    }
}

void smp_initialize(void)
{
    log("smp_initialize...");
    for (size_t cpu = 0; cpu < cpu_count(); cpu++)
    {
        if (cpu == apic_current_cpu())
        {
            continue; // don't init current cpu
        }

        log("smp_initialize(): loading cpu {}", cpu);

        cpu_started = true;
        smp_initialize_cpu(cpu, cpu_context(cpu)->lapic);
        wait_for_cpu();
    }
}