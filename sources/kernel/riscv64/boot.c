#include <brutal/debug.h>
#include <hw/fdt/fdt.h>
#include "kernel/arch.h"
#include "kernel/riscv64/arch.h"
#include "kernel/riscv64/interrupts.h"
#include "kernel/riscv64/uart8250.h"

GenericUartDevice *_uart_device = NULL;

void arch_entry_main(uint64_t hart_id, uint64_t fdt_addr)
{
    _uart_device = uart8250_init();
    set_arch_uart_device(_uart_device);

    log$("started cpu: {} with fdt: {}", hart_id, fdt_addr);

    // sifive_uart_init();
    FdtHeader *header = (FdtHeader *)fdt_from_data((void *)fdt_addr);
    Emit target;
    emit_init(&target, arch_debug());
    fdt_dump(header, &target);

    log$("booting...");
    init_interrupts();
    log$("loaded interrupts");
    while (1)
    {
    }
}
