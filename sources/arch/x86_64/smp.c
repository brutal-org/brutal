#include <brutal/log.h>
#include <brutal/mem.h>
#include "arch/arch.h"
#include "arch/heap.h"
#include "arch/mmio.h"
#include "arch/pmm.h"
#include "arch/vmm.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/memory/mmap.h"
#include "arch/x86_64/pit.h"
#include "arch/x86_64/smp.h"
#include "host/mem.h"
#include "kernel/constants.h"

typedef result_t(br_error_t, uint8_t) smp_init_result;

atomic_bool cpu_ready = false;

extern uint32_t trampoline_start;
extern uint32_t trampoline_end;

static size_t smp_cpu_trampoline_size(void)
{
    uint64_t trampoline_len = (uintptr_t)&trampoline_end - (uintptr_t)&trampoline_start + HOST_MEM_PAGESIZE;
    return ALIGN_UP(trampoline_len, HOST_MEM_PAGESIZE);
}

static smp_init_result smp_initialize_cpu_trampoline(void)
{
    log("Initializing cpu trampoline");
    log("trampoline is {x}", smp_cpu_trampoline_size());

    // Map everything under the trampoline
    uint64_t trampoline_len = (uintptr_t)&trampoline_end - (uintptr_t)&trampoline_start + HOST_MEM_PAGESIZE;

    vmm_map(vmm_kernel_space(),
            (vmm_range_t){
                .base = (0x0),
                .size = smp_cpu_trampoline_size()},
            (pmm_range_t){
                .base = (0x0),
                .size = smp_cpu_trampoline_size()},
            BR_MEM_WRITABLE);

    vmm_space_switch(vmm_kernel_space());

    mem_cpy((void *)0x1000, (void *)&trampoline_start, trampoline_len - 0x1000);

    return OK(smp_init_result, 0);
}

static void smp_cleanup_cpu_trampoline(void)
{
    log("Cleaning up cpu trampoline");
    // Unmap everything under the trampoline

    mem_set(0x0, 0x69, smp_cpu_trampoline_size());

    vmm_unmap(vmm_kernel_space(),
              (vmm_range_t){
                  .base = (0x0),
                  .size = smp_cpu_trampoline_size(),
              });
}

void smp_entry_other(void)
{
    cpu_ready = true;
    arch_entry_other();
}

static smp_init_result smp_initialize_cpu_data(void)
{
    // load future cpu page table
    mmio_write64(SMP_INIT_PAGE_TABLE, asm_read_cr3());

    // load future cpu stack
    uint8_t *cpu_stack = (uint8_t *)TRY(smp_init_result, heap_alloc(KERNEL_STACK_SIZE)).base;
    mmio_write64(SMP_INIT_STACK, (uint64_t)(cpu_stack + KERNEL_STACK_SIZE));

    asm volatile(
        "sgdt 0x580\n"   // gdt at 0x580
        "sidt 0x590\n"); // idt at 0x590

    mmio_write64(SMP_INIT_ENTRY, (uintptr_t)smp_entry_other);
    return OK(smp_init_result, 0);
}

static void smp_initialize_cpu(uint32_t lapic_id)
{
    smp_initialize_cpu_data();
    apic_init_processor(lapic_id);
    pit_sleep(10);
    apic_start_processor(lapic_id, 4096);
}

void arch_boot_other(void)
{
    // load future cpu page table
    smp_initialize_cpu_trampoline();

    log("Booting other CPUs...");

    for (size_t cpu = 0; cpu < cpu_count(); cpu++)
    {
        if (cpu == apic_current_cpu())
        {
            continue; // don't init current cpu
        }

        cpu_ready = false;

        log("Bootings CPU N°{}...", cpu);
        smp_initialize_cpu(cpu_impl(cpu)->lapic);

        WAIT_FOR(cpu_ready);
    }

    smp_cleanup_cpu_trampoline();
}
