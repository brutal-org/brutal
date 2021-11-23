

#include <brutal/debug.h>
#include "kernel/riscv64/fdt.h"
#include "kernel/riscv64/interrupts.h"
#include "kernel/riscv64/uart8250.h"

void arch_entry_main(uint64_t hart_id, uint64_t fdt_addr)

{
    uart8250_init();
    log$("started cpu: {} with fdt: {}", hart_id, fdt_addr);

    // sifive_uart_init();
    FdtHeader *header = (FdtHeader *)fdt_addr;
    FdtBeginNode *node = fdt_root(header);
    dump_fdt(header, node);

    node = get_fdt_node((node), str$("soc"));
    node = get_fdt_node((node), str$("uart@10000000"));
    log$("booting...");
    init_interrupts();
    log$("loaded interrupts");
    while (1)
    {
    }
}
