#include <brutal/mem.h>
#include <hw/uart/uart.h>

void uart_putc(GenericUartDevice *dev, uint8_t data)
{
    if (dev->byte_size == 1)
    {
        volatile_write8(dev->tx_addr, data);
    }
    else if (dev->byte_size == 2)
    {
        volatile_write16(dev->tx_addr, data);
    }
    else if (dev->byte_size == 4)
    {
        volatile_write32(dev->tx_addr, data);
    }
}

uint8_t uart_getc(GenericUartDevice *dev)
{
    if (dev->byte_size == 1)
    {
        return volatile_read8(dev->rx_addr);
    }
    else if (dev->byte_size == 2)
    {
        return volatile_read16(dev->rx_addr);
    }
    else if (dev->byte_size == 4)
    {
        return volatile_read32(dev->rx_addr);
    }
    return 0;
}

static IoResult arch_debug_write(void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        uart_putc(context, data[i]);
    }

    return OK(IoResult, size);
}

IoWriter uart_writer(GenericUartDevice *dev)
{
    return (IoWriter){
        .write = arch_debug_write,
        .context = dev,
    };
}
