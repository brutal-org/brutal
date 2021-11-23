#include <brutal/mem.h>
#include "kernel/riscv64/platform.h"

void sifive_uart_init(void)
{
    // TODO: set baud + gpio (maybe ?)
    volatile_write32(SIFIVE_UART0_BASE + SIFIVE_UART_TX_CTRL, volatile_read32(SIFIVE_UART0_BASE + SIFIVE_UART_TX_CTRL) | SIFIVE_UART_TX_ENABLE);
}

void sifive_uart_putc(char v)
{

    while (volatile_read32(SIFIVE_UART0_BASE + SIFIVE_UART_TX_DATA) & SIFIVE_UART_TX_FULL)
    {
    }

    volatile_write32(SIFIVE_UART0_BASE + SIFIVE_UART_TX_DATA, v);
}
