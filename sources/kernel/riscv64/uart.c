#include <brutal/mem.h>
#include "kernel/riscv64/platform.h"

void uart_init(void)
{
    // TODO: set baud + gpio (maybe ?)
    volatile_write32(UART0_BASE + UART_TX_CTRL, volatile_read32(UART0_BASE + UART_TX_CTRL) | UART_TX_ENABLE);
}

void uart_putc(char v)
{

    while (volatile_read32(UART0_BASE + UART_TX_DATA) & UART_TX_FULL)
    {
    }

    volatile_write32(UART0_BASE + UART_TX_DATA, v);
}
