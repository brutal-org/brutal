

#include <brutal/debug.h>
#include "kernel/riscv64/interrupts.h"
#include "kernel/riscv64/uart.h"

void arch_entry_main(void)
{
    uart_init();
    log$("booting...");
    init_interrupts();
    log$("loaded interrupts");
    while (1)
    {
    }
}
