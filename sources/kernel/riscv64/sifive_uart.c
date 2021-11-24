#include <brutal/mem.h>
#include "kernel/riscv64/platform.h"
#include "kernel/riscv64/sifive_uart.h"

GenericUartDevice _self;

GenericUartDevice *sifive_uart_init(void)
{
    // TODO: set baud + gpio (maybe ?)
    volatile_write32(SIFIVE_UART0_BASE + SIFIVE_UART_TX_CTRL, volatile_read32(SIFIVE_UART0_BASE + SIFIVE_UART_TX_CTRL) | SIFIVE_UART_TX_ENABLE);

    _self.addr = SIFIVE_UART0_BASE;
    _self.byte_size = sizeof(uint32_t);
    return &_self;
}
