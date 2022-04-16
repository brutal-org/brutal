#pragma once

#include <brutal-io>

typedef struct
{
    uint8_t byte_size;
    uintptr_t tx_addr;
    uintptr_t rx_addr;
} Uart;

void uart_putc(Uart *dev, uint8_t data);
uint8_t uart_getc(Uart *dev);

IoWriter uart_writer(Uart *dev);
