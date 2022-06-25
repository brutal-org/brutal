#include <brutal-debug>
#include <brutal-mem>
#include <embed/mem.h>

#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/cpu.h"
#include "kernel/x86_64/hpet.h"
#include "kernel/x86_64/smp.h"

#include "kernel/arch.h"
#include "kernel/heap.h"
#include "kernel/kernel.h"
#include "kernel/mmap.h"
#include "kernel/pmm.h"
#include "kernel/vmm.h"

static atomic_bool _ready = false;

extern uint32_t trampoline_start;
extern uint32_t trampoline_end;

#define TRAMPOLINE_VIRTUAL_START (0x1000)

static size_t smp_trampoline_size(void)
{
    uint64_t trampoline_len = (uintptr_t)&trampoline_end - (uintptr_t)&trampoline_start;
    return align_up$(trampoline_len, MEM_PAGE_SIZE);
}

static void smp_trampoline_map(void)
{
    log$("Initializing cpu trampoline");
    log$("Trampoline is {x} bytes in size", smp_trampoline_size());

    uint64_t trampoline_len = smp_trampoline_size();

    VmmRange target_range = (VmmRange){
        .base = (0x0),
        .size = trampoline_len + TRAMPOLINE_VIRTUAL_START, /* map under the trampoline for cpu cpu magic value (see smp_initialize_cpu_contex) */
    };

    vmm_map(vmm_kernel_space(),
            target_range,
            (PmmRange){
                .base = (0x0),
                .size = trampoline_len + TRAMPOLINE_VIRTUAL_START,
            },
            BR_MEM_WRITABLE);

    vmm_flush(vmm_kernel_space(), target_range);

    mem_cpy((void *)TRAMPOLINE_VIRTUAL_START, (void *)&trampoline_start, trampoline_len);
}

static void smp_trampoline_unmap(void)
{
    log$("Cleaning up cpu trampoline");

    vmm_unmap(vmm_kernel_space(),
              (VmmRange){
                  .base = (0x0),
                  .size = smp_trampoline_size() + TRAMPOLINE_VIRTUAL_START,
              });
}

void smp_entry_other(void)
{
    _ready = true;
    arch_entry_other();
}

static void smp_initialize_cpu_context(void)
{
    // Load future cpu page table

    volatile_write32(SMP_INIT_PAGE_TABLE, asm_read_cr3());

    // Load future cpu stack
    uint8_t *cpu_stack = (uint8_t *)(UNWRAP(heap_alloc_lower(KERNEL_STACK_SIZE)).base);
    volatile_write64(SMP_INIT_STACK, (uint64_t)((uintptr_t)cpu_stack + KERNEL_STACK_SIZE));

    asm volatile(
        "sgdt 0x530\n"
        "sidt 0x540\n");

    volatile_write64(SMP_INIT_ENTRY, (uintptr_t)smp_entry_other);
}

static void smp_start_cpu(uint32_t lapic_id)
{
    lapic_send_init(lapic_id);

    hpet_sleep(10);

    lapic_send_sipi(lapic_id, TRAMPOLINE_VIRTUAL_START);
}

void smp_boot_other(void)
{
    smp_trampoline_map();

    log$("Booting other CPUs...");

    for (int cpu = 0; cpu < cpu_count(); cpu++)
    {
        if (cpu == cpu_self_id())
        {
            continue; // don't init current cpu
        }

        log$("Bootings CPU N°{}...", cpu);

        _ready = false;

        smp_initialize_cpu_context();
        smp_start_cpu(cpu_impl(cpu)->lapic);

        wait_for$(_ready);
    }

    smp_trampoline_unmap();
}

void smp_stop_all(void)
{
    for (int i = 0; i < cpu_count(); i++)
    {
        if (i != cpu_self_id())
        {
            lapic_send_ipi(cpu_impl(i)->lapic, IPI_STOP);
        }
    }
}
