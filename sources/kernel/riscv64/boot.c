#include <bal/boot.h>
#include <fdt/fdt.h>
#include <fdt/handover.h>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-ds>
#include "kernel/arch.h"
#include "kernel/pmm.h"
#include "kernel/riscv64/arch.h"
#include "kernel/riscv64/interrupts.h"
#include "kernel/riscv64/uart8250.h"
#include "kernel/vmm.h"

static Uart *_uart = NULL;
static Handover _handover = {};

void arch_entry_main(uint64_t hart_id, uint64_t fdt_addr, uint64_t kstart, uint64_t kend)
{
    _uart = uart8250_init();
    arch_use_uart(_uart);

    log$("Started hart: {} with fdt: {} ({}-{})", hart_id, fdt_addr, (uintptr_t)kstart, (uintptr_t)kend);

    FdtHeader *header = (FdtHeader *)fdt_from_data((void *)fdt_addr);
    Emit target;
    emit_init(&target, arch_debug());
    fdt_dump(header, &target, false);

    init_interrupts();

    HandoverMmapEntry kernel_entry = {
        .base = (uintptr_t)kstart,
        .size = (uintptr_t)kend - (uintptr_t)kstart,
        .type = HANDOVER_MMAP_USED,
    };

    fdt_populate_handover(header, &_handover);
    handover_mmap_append(&_handover.mmap, kernel_entry);

    handover_dump(&_handover);

    pmm_initialize(&_handover);
    vmm_initialize(&_handover);
    while (1)
    {
    }
}
