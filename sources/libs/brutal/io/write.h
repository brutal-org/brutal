#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>
#include <brutal/text/str.h>

typedef Result(Error, size_t) IoWriteResult;

typedef IoWriteResult IoWrite(void *context, uint8_t const *data, size_t offset, size_t size);

typedef struct
{
    IoWrite *write;
    void *context;
    size_t offset;
} IoWriter;

static inline IoWriteResult io_write(IoWriter *self, uint8_t *data, size_t size)
{
    size_t written = TRY(IoWriteResult, self->write(self->context, data, self->offset, size));
    self->offset += written;
    return OK(IoWriteResult, written);
}

IoWriteResult io_put(IoWriter *writer, uint8_t c);

IoWriteResult io_print(IoWriter *writer, Str str);
