#pragma once

#include <brutal-base>

enum com_port
{
    COM1 = 0x3F8,
    COM2 = 0x2F8,
    COM3 = 0x3E8,
    COM4 = 0x2E8,
};

enum com_register
{
    COM_DATA = 0,
    COM_INTERRUPT = 1,
    COM_BAUD_RATE_LOW = 0,
    COM_BAUD_RATE_HIGH = 1,
    COM_INTERRUPT_IDENTIFICATOR = 2,
    COM_FIFO_CONTROLLER = 2,
    COM_LINE_CONTROL = 3,
    COM_MODEM_CONTROL = 4,
    COM_LINE_STATUS = 5,
    COM_MODEM_STATUS = 6,
    COM_SCRATCH_REGISTER = 7,
};

enum com_line_control_bit
{
    COM_DATA_SIZE_5 = 0,
    COM_DATA_SIZE_6 = 1,
    COM_DATA_SIZE_7 = 2,
    COM_DATA_SIZE_8 = 3,
    COM_DLAB_STATUS = 1 << 7,
};

enum com_modem_bit
{
    COM_MODEM_DTR = 1 << 0,
    COM_MODEM_RTS = 1 << 1,
    COM_MODEM_OUT1 = 1 << 2,
    COM_MODEM_OUT2 = 1 << 3,
    COM_MODEM_LOOPBACK = 1 << 4,
};

enum com_interrupt_enable_bit
{
    COM_INTERRUPT_WHEN_DATA_AVAILABLE = 1 << 0,
    COM_INTERRUPT_WHEN_TRANSMITTER_EMPTY = 1 << 1,
    COM_INTERRUPT_WHEN_BREAK_ERROR = 1 << 2,
    COM_INTERRUPT_WHEN_STATUS_UPDATE = 1 << 3,
};

enum com_line_status_bit
{
    COM_LINE_DATA_READY = 1 << 0,
    COM_LINE_OVERRUN_ERROR = 1 << 1,
    COM_LINE_PARITY_ERROR = 1 << 2,
    COM_LINE_FRAMING_ERROR = 1 << 3,
    COM_LINE_BREAK_INDICATOR = 1 << 4,
    COM_LINE_TRANSMITTER_BUF_EMPTY = 1 << 5,
    COM_LINE_TRANSMITTER_EMPTY = 1 << 6,
    COM_LINE_IMPENDING_ERROR = 1 << 7,
};

void com_write_reg(enum com_port port, enum com_register reg, uint8_t value);

uint8_t com_read_reg(enum com_port port, enum com_register reg);

bool com_can_read(enum com_port port);

bool com_can_write(enum com_port port);

void com_wait_write(enum com_port port);

void com_wait_read(enum com_port port);

void com_putc(enum com_port port, char c);

char com_getc(enum com_port port);

size_t com_write(enum com_port port, void const *buf, size_t size);

void com_initialize(enum com_port port);
