#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>
#include <brutal/io/base.h>
#include <brutal/text/str.h>

typedef IoResult IoWrite(void *context, uint8_t const *data, size_t offset, size_t size);

typedef struct
{
    IoWrite *write;
    void *context;
    size_t offset;
} IoWriter;

static inline IoResult io_write(IoWriter *self, const uint8_t *data, size_t size)
{
    size_t written = TRY(IoResult, self->write(self->context, data, self->offset, size));
    self->offset += written;
    return OK(IoResult, written);
}

IoResult io_put(IoWriter *writer, uint8_t c);

IoResult io_print(IoWriter *writer, Str str);
