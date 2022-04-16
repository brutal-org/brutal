#include "kernel/riscv64/uart8250.h"
#include <brutal-base>
#include <brutal-mem>

static Uart _self;

void uart8250_write(uint16_t reg, uint8_t value)
{
    volatile_write8(UART8250_BASE + reg, value);
}
uint8_t uart8250_read(uint16_t reg)
{
    return volatile_read8(UART8250_BASE + reg);
}

Uart *uart8250_init(void)
{
    uart8250_write(UART_8250_INTERRUPT_ENABLE, 0);
    uart8250_write(UART_8250_LINE_CONTROL, 0x03);
    uart8250_write(UART_8250_FIFO_CONTROL, 1);
    uart8250_write(UART_8250_MODEM_CONTROL, 0);
    uart8250_read(UART_8250_LINE_STATUS); // clear line status and buffer
    uart8250_read(UART_8250_RECEIVER_BUFFER);

    _self.byte_size = 1;
    _self.tx_addr = UART8250_BASE + UART_8250_TRASMITTER_HOLDING_BUFFER;
    _self.rx_addr = UART8250_BASE + UART_8250_RECEIVER_BUFFER;
    return &_self;
}
