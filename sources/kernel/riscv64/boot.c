#include <bal/boot.h>
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

static Uart *_uart = NULL;
static Handover _handover = {};

extern uintptr_t _kernel_end;
extern uintptr_t _kernel_start;

void arch_entry_main(uint64_t hart_id, uint64_t fdt_addr)
{
    _uart = uart8250_init();
    arch_use_uart(_uart);

    log$("Started hart: {} with fdt: {}", hart_id, fdt_addr);

    FdtHeader *header = (FdtHeader *)fdt_from_data((void *)fdt_addr);
    Emit target;
    emit_init(&target, arch_debug());
    fdt_dump(header, &target, false);

    init_interrupts();

    HandoverMmapEntry kernel_entry = {
        .base = (uintptr_t)&_kernel_start,
        .size = (uintptr_t)&_kernel_end - (uintptr_t)&_kernel_start,
        .type = HANDOVER_MMAP_USED,
    };

    fdt_populate_handover(header, &_handover);
    handover_mmap_append(&_handover.mmap, kernel_entry);

    handover_dump(&_handover);

    pmm_initialize(&_handover);
    while (1)
    {
    }
}
