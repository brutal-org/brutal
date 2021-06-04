#include "arch/com.h"
#include "arch/asm.h"

void com_write(enum com_port port, uint16_t reg, uint8_t value)
{
    out8(port + reg, value);
}

uint8_t com_read(enum com_port port, uint16_t reg)
{
    return in8(port + reg);
}

bool com_can_read(enum com_port port)
{
    return (com_read(port, COM_LINE_STATUS) & COM_LINE_DATA_READY);
}

bool com_can_write(enum com_port port)
{
    return (com_read(port, COM_LINE_STATUS) & COM_LINE_TRANSMITTER_BUFFER_EMPTY);
}

void com_wait_write(enum com_port port)
{
    while (!com_can_write(port))
    { /* do nothing */
    }
}

void com_wait_read(enum com_port port)
{
    while (!com_can_read(port))
    { /* do nothing */
    }
}

void com_putc(enum com_port port, char c)
{
    com_wait_write(port);
    com_write(port, COM_DATA, c);
}

char com_getc(enum com_port port)
{
    com_wait_read(port);
    return com_read(port, COM_DATA);
}

size_t com_write_str(enum com_port port, const void *buffer, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        com_putc(port, ((const char *)buffer)[i]);
    }

    return size;
}

void com_initialize(enum com_port port)
{
    com_write(port, COM_INTERRUPT_IDENTIFICATOR, 0);

    com_write(port, COM_LINE_CONTROL, COM_DLAB_STATUS); // turn on dlab for setting baud rate
    
    com_write(port, COM_BAUD_RATE_LOW, 115200 / 9600); // set bauds
    com_write(port, COM_BAUD_RATE_HIGH, 0);


    com_write(port, COM_LINE_CONTROL, COM_DATA_SIZE_8); // we want 8bit caracters + clear dlab
        
    com_write(port, COM_MODEM_CONTROL, COM_MODEM_DTR | COM_MODEM_RTS | COM_MODEM_OUT2); // turn on communication + redirect UART interrupt into ICU

    com_write(port, COM_INTERRUPT, COM_INTERRUPT_WHEN_DATA_AVAILABLE);
}