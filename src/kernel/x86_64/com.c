#include "kernel/x86_64/com.h"

#include "kernel/x86_64/asm.h"

void com_write_reg(enum com_port port, enum com_register reg, uint8_t value)
{
    asm_out8(port + reg, value);
}

uint8_t com_read_reg(enum com_port port, enum com_register reg)
{
    return asm_in8(port + reg);
}

bool com_can_read(enum com_port port)
{
    return (com_read_reg(port, COM_LINE_STATUS) & COM_LINE_DATA_READY);
}

bool com_can_write(enum com_port port)
{
    return (com_read_reg(port, COM_LINE_STATUS) & COM_LINE_TRANSMITTER_BUF_EMPTY);
}

void com_wait_write(enum com_port port)
{
    wait_for$(com_can_write(port));
}

void com_wait_read(enum com_port port)
{
    wait_for$(com_can_read(port))
}

void com_putc(enum com_port port, char c)
{
    com_wait_write(port);
    com_write_reg(port, COM_DATA, c);
}

char com_getc(enum com_port port)
{
    com_wait_read(port);
    return com_read_reg(port, COM_DATA);
}

size_t com_write(enum com_port port, void const *buf, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        com_putc(port, ((char const *)buf)[i]);
    }

    return size;
}

void com_initialize(enum com_port port)
{
    com_write_reg(port, COM_INTERRUPT_IDENTIFICATOR, 0);

    // Turn on dlab for setting baud rate
    com_write_reg(port, COM_LINE_CONTROL, COM_DLAB_STATUS);

    // Set bauds
    com_write_reg(port, COM_BAUD_RATE_LOW, 115200 / 9600);
    com_write_reg(port, COM_BAUD_RATE_HIGH, 0);

    // we want 8bit characters + clear dlab
    com_write_reg(port, COM_LINE_CONTROL, COM_DATA_SIZE_8);

    // turn on communication + redirect UART interrupt into ICU
    com_write_reg(port, COM_MODEM_CONTROL, COM_MODEM_DTR | COM_MODEM_RTS | COM_MODEM_OUT2);
    com_write_reg(port, COM_INTERRUPT, COM_INTERRUPT_WHEN_DATA_AVAILABLE);
}
