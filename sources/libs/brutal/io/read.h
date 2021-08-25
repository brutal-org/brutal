#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>

typedef Result(Error, size_t) IoReadResult;

typedef IoReadResult IoRead(void *context, uint8_t *data, size_t offset, size_t size);

typedef struct
{
    IoRead *read;
    void *context;
    size_t offset;
} IoReader;

static inline IoReadResult io_read(IoReader *self, uint8_t *data, size_t size)
{
    size_t read = TRY(IoReadResult, self->read(self->context, data, self->offset, size));
    self->offset += read;
    return OK(IoReadResult, read);
}
