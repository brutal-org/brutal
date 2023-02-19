#include "kernel/riscv64/sifive_uart.h"
#include <brutal-mem>

static Uart _self;

Uart *sifive_uart_init(void)
{
    // TODO: set baud + gpio (maybe ?)
    volatile_write32(SIFIVE_UART0_BASE + SIFIVE_UART_TX_CTRL, volatile_read32(SIFIVE_UART0_BASE + SIFIVE_UART_TX_CTRL) | SIFIVE_UART_TX_ENABLE);

    _self.tx_addr = SIFIVE_UART0_BASE + SIFIVE_UART_TX_DATA;
    _self.rx_addr = SIFIVE_UART0_BASE + SIFIVE_UART_RX_DATA;
    _self.byte_size = sizeof(uint32_t);
    return &_self;
}
