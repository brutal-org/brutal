#pragma once

#include <brutal/io/base.h>

typedef IoResult IoWriteFn(void *context, uint8_t const *data, size_t size);

typedef struct
{
    IoWriteFn *write;
    void *context;
} IoWriter;

IoResult io_write(IoWriter *self, uint8_t const *data, size_t size);

IoResult io_put(IoWriter *writer, uint8_t c);

IoResult io_print(IoWriter *writer, Str str);
