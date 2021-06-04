#pragma once

#include <library/base.h>

enum com_port
{
    COM1 = 0x3F8,
    COM2 = 0x2F8,
    COM3 = 0x3E8,
    COM4 = 0x2E8,
};

bool com_can_read(enum com_port port);

bool com_can_write(enum com_port port);

void com_wait_write(enum com_port port);

void com_wait_read(enum com_port port);

void com_putc(enum com_port port, char c);

char com_getc(enum com_port port);

size_t com_write_str(enum com_port port, const void *buffer, size_t size);

void com_initialize(enum com_port port);