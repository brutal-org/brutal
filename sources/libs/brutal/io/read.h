#pragma once

#include <brutal/io/base.h>

typedef IoResult IoReadFn(void *context, uint8_t *data, size_t size);

typedef struct
{
    IoReadFn *read;
    void *context;
} IoReader;

IoResult io_read(IoReader *self, uint8_t *data, size_t size);
