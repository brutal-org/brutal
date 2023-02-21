#include <brutal/debug/log.h>
#include "kernel/core/arch.h"
#include "kernel/core/event.h"
#include "kernel/core/init.h"
#include "kernel/core/kernel.h"
#include "kernel/core/mmap.h"
#include "kernel/core/pmm.h"
#include "kernel/core/sched.h"
#include "kernel/core/tasking.h"
#include "kernel/core/vmm.h"
#include "kernel/arch/x86_64/apic.h"
#include "kernel/arch/x86_64/asm.h"
#include "kernel/arch/x86_64/com.h"
#include "kernel/arch/x86_64/cpu.h"
#include "kernel/arch/x86_64/gdt.h"
#include "kernel/arch/x86_64/hpet.h"
#include "kernel/arch/x86_64/idt.h"
#include "kernel/arch/x86_64/pic.h"
#include "kernel/arch/x86_64/simd.h"
#include "kernel/arch/x86_64/smp.h"
#include "kernel/arch/x86_64/syscall.h"

static atomic_int _ready = 0;

void arch_entry_main(Handover *handover, uint64_t magic)
{
    cpu_disable_interrupts();
    cpu_retain_disable();

    com_initialize(COM1);

    kernel_splash();

    log$("Handover at {#p} with magic {08x}", (void *)handover, magic);
    handover_dump(handover);

    gdt_initialize();
    idt_initialize();
    simd_initialize();
    pmm_initialize(handover);
    vmm_initialize(handover);

    Acpi acpi = {};
    acpi_init(&acpi, MMAP_IO_BASE, handover->rsdp);

    hpet_initialize(&acpi);
    apic_initalize(&acpi);
    cpu_initialize();
    syscall_initialize();
    sched_initialize();
    event_initialize();
    tasking_initialize();

    _ready++;
    smp_boot_other();
    wait_for$(_ready == cpu_count());

    cpu_retain_enable();
    cpu_enable_interrupts();

    init_start(handover);

    sched_stop(task_self(), 0);
    panic$("sched_stop(self) returned!");
}

void arch_entry_other(void)
{
    cpu_disable_interrupts();
    cpu_retain_disable();

    lapic_enable();
    simd_initialize();
    cpu_initialize();
    syscall_initialize();

    _ready++;
    wait_for$(_ready == cpu_count());

    cpu_retain_enable();
    cpu_enable_interrupts();

    sched_stop(task_self(), 0);
    panic$("sched_stop(self) returned!");
}
