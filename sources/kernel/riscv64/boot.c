

#include <brutal/debug.h>
#include "kernel/riscv64/fdt.h"
#include "kernel/riscv64/interrupts.h"
#include "kernel/riscv64/uart8250.h"

void arch_entry_main(uint64_t hart_id, uint64_t fdt_addr)

{
    uart8250_init();
    log$("booting...");
    init_interrupts();
    log$("loaded interrupts");
    while (1)
    {
    }
}
