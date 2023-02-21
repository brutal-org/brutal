#pragma once

#include <uart/uart.h>

// todo: find uart addr from device tree
#define SIFIVE_UART0_BASE 0x10010000
#define SIFIVE_UART1_BASE 0x10011000

enum sifive_uart_registers
{
    SIFIVE_UART_TX_DATA = 0,
    SIFIVE_UART_RX_DATA = 4,
    SIFIVE_UART_TX_CTRL = 8,
    SIFIVE_UART_RX_CTRL = 12,
    SIFIVE_UART_INT_ENABLE = 16,
    SIFIVE_UART_INT_PENDING = 20,
    SIFIVE_UART_BAUD_DIV = 24,
};

#define SIFIVE_UART_TX_TRANSMIT 0xff
#define SIFIVE_UART_TX_FULL (1 << 31)

#define SIFIVE_UART_RX_RECEIVE 0xff
#define SIFIVE_UART_RX_EMPTY (1 << 31)

enum sifive_uart_tx_control_bit
{
    SIFIVE_UART_TX_ENABLE = (1 << 0),     // size: 1
    SIFIVE_UART_TX_STOP_NUM = (1 << 1),   // size: 1
    SIFIVE_UART_TX_WATERMARK = (1 << 16), // size: 2
};

enum sifive_uart_rx_control_bit
{
    SIFIVE_UART_RX_ENABLE = (1 << 0),
    SIFIVE_UART_RX_WATERMARK = (1 << 16),
};

enum sifive_uart_int_enable_bit
{
    SIFIVE_UART_INT_TX_WATERMARK_ENABLE = (1 << 0),
    SIFIVE_UART_INT_RX_WATERMARK_DISABLE = (1 << 1),
};

enum sifive_uart_int_pending_bit
{
    SIFIVE_UART_INT_TX_WATERMARK_PENDING = (1 << 0),
    SIFIVE_UART_INT_RX_WATERMARK_PENDING = (1 << 1),
};

#define SIFIVE_UART_BAUD_DIV_BIT (0xfffff)

Uart *sifive_uart_init(void);
