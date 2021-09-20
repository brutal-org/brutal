#pragma once

#define UART0_BASE 0x10010000
#define UART1_BASE 0x10011000

enum uart_registers
{
    UART_TX_DATA = 0,
    UART_RX_DATA = 4,
    UART_TX_CTRL = 8,
    UART_RX_CTRL = 12,
    UART_INT_ENABLE = 16,
    UART_INT_PENDING = 20,
    UART_BAUD_DIV = 24,
};

#define UART_TX_TRANSMIT 0xff
#define UART_TX_FULL (1 << 31)

#define UART_RX_RECEIVE 0xff
#define UART_RX_EMPTY (1 << 31)

enum uart_tx_control_bit
{
    UART_TX_ENABLE = (1 << 0),     // size: 1
    UART_TX_STOP_NUM = (1 << 1),   // size: 1
    UART_TX_WATERMARK = (1 << 16), // size: 2
};

enum uart_rx_control_bit
{
    UART_RX_ENABLE = (1 << 0),
    UART_RX_WATERMARK = (1 << 16),
};

enum uart_int_enable_bit
{
    UART_INT_TX_WATERMARK_ENABLE = (1 << 0),
    UART_INT_RX_WATERMARK_DISABLE = (1 << 1),
};

enum uart_int_pending_bit
{
    UART_INT_TX_WATERMARK_PENDING = (1 << 0),
    UART_INT_RX_WATERMARK_PENDING = (1 << 1),
};

#define UART_BAUD_DIV_BIT (0xfffff)
