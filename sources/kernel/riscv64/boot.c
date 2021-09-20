

#include "kernel/riscv64/uart.h"
void arch_entry_main(void)
{
    uart_init();
    uart_putc('h');
    uart_putc('i');
    uart_putc('\n');
    while (1)
    {
    }
}
