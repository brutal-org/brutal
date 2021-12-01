#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/ds.h>
#include <hw/fdt/fdt.h>
#include "kernel/arch.h"
#include "kernel/pmm.h"
#include "kernel/riscv64/arch.h"
#include "kernel/riscv64/handover.h"
#include "kernel/riscv64/interrupts.h"
#include "kernel/riscv64/uart8250.h"
GenericUartDevice *_uart_device = NULL;
void arch_entry_main(uint64_t hart_id, uint64_t fdt_addr)
{
    _uart_device = uart8250_init();
    set_arch_uart_device(_uart_device);

    log$("started cpu: {} with fdt: {}", hart_id, fdt_addr);

    FdtHeader *header = (FdtHeader *)fdt_from_data((void *)fdt_addr);
    Emit target;
    emit_init(&target, arch_debug());
    fdt_dump(header, &target, false);

    log$("booting...");
    init_interrupts();
    log$("loaded interrupts");

    Handover v = arch_create_handover(fdt_addr);
    handover_dump(&v);

    pmm_initialize(&v);
    while (1)
    {
    }
}
