#pragma once

#include <brutal/io/write.h>
#include <stdint.h>

typedef struct
{
    uint8_t byte_size;
    uintptr_t tx_addr;
    uintptr_t rx_addr;
} GenericUartDevice;

void uart_putc(GenericUartDevice *dev, uint8_t data);
uint8_t uart_getc(GenericUartDevice *dev);

IoWriter uart_writer(GenericUartDevice *dev);
