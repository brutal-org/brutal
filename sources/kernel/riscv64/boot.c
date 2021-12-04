#include <bal/boot/handover.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/ds.h>
#include <fdt/fdt.h>
#include <fdt/handover.h>
#include "kernel/arch.h"
#include "kernel/pmm.h"
#include "kernel/riscv64/arch.h"
#include "kernel/riscv64/interrupts.h"
#include "kernel/riscv64/uart8250.h"

static Uart *_uart_device = NULL;
static Handover boot_handover = {};

extern uintptr_t _kernel_end;
extern uintptr_t _kernel_start;

void arch_entry_main(uint64_t hart_id, uint64_t fdt_addr)
{
    _uart_device = uart8250_init();
    arch_use_uart(_uart_device);

    log$("started cpu: {} with fdt: {}", hart_id, fdt_addr);

    FdtHeader *header = (FdtHeader *)fdt_from_data((void *)fdt_addr);
    Emit target;
    emit_init(&target, arch_debug());
    fdt_dump(header, &target, false);

    log$("booting...");
    init_interrupts();
    log$("loaded interrupts");

    HandoverMmapEntry kernel_entry = {
        .base = (uintptr_t)&_kernel_start,
        .size = (uintptr_t)&_kernel_end - (uintptr_t)&_kernel_start,
        .type = HANDOVER_MMAP_USED,
    };

    fdt_populate_handover(header, &boot_handover);
    handover_mmap_append(&boot_handover.mmap, kernel_entry);

    handover_dump(&boot_handover);

    pmm_initialize(&boot_handover);
    while (1)
    {
    }
}
