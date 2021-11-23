#include "kernel/riscv64/uart8250.h"
#include <brutal/base.h>
#include <brutal/mem.h>

// todo: parse device tree for finding uart8250 base addr

/*
        uart@10000000 {
            interrupts = <0x0a>;
            interrupt-parent = <0x09>;
            clock-frequency = "\08@";
            reg = <0x00 0x10000000 0x00 0x100>;
            compatible = "ns16550a";
        };
*/

#define UART8250_BASE 0x10000000

void uart8250_write(uint16_t reg, uint8_t value)
{
    volatile_write8(UART8250_BASE + reg, value);
}
uint8_t uart8250_read(uint16_t reg)
{
    return volatile_read8(UART8250_BASE + reg);
}

void uart8250_init(void)
{
    uart8250_write(UART_8250_INTERRUPT_ENABLE, 0);
    uart8250_write(UART_8250_LINE_CONTROL, 0x03);
    uart8250_write(UART_8250_FIFO_CONTROL, 1);
    uart8250_write(UART_8250_MODEM_CONTROL, 0);
    uart8250_read(UART_8250_LINE_STATUS); // clear line status and buffer
    uart8250_read(UART_8250_RECEIVER_BUFFER);
}

void uart8250_putc(char c)
{
    while ((uart8250_read(UART_8250_LINE_STATUS) & UART_8250_LSR_TRANSMITTER_HOLD_REG_EMPTY) == 0)
        ;

    uart8250_write(UART_8250_TRASMITTER_HOLDING_BUFFER, c);
}
