#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>
#include <brutal/io/base.h>

typedef IoResult IoRead(void *context, uint8_t *data, size_t offset, size_t size);

typedef struct
{
    IoRead *read;
    void *context;
    size_t offset;
} IoReader;

static inline IoResult io_read(IoReader *self, uint8_t *data, size_t size)
{
    size_t read = TRY(IoResult, self->read(self->context, data, self->offset, size));
    self->offset += read;
    return OK(IoResult, read);
}
