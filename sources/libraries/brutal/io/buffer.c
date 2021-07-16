#include <brutal/io/buffer.h>
#include "brutal/base/macros.h"

void buffer_init(Buffer *self, size_t capacity, Alloc *alloc)
{
    *self = (Buffer){
        .data = nullptr,
        .used = 0,
        .capacity = 0,
        .alloc = alloc,
    };

    buffer_ensure(self, capacity);
}

void buffer_deinit(Buffer *self)
{
    alloc_free(self->alloc, self->data);
    *self = (Buffer){};
}

void buffer_ensure(Buffer *self, size_t capacity)
{
    if (self->capacity >= capacity)
    {
        return;
    }

    if (self->capacity == 0)
    {
        self->data = alloc_calloc(self->alloc, 1, capacity);
        self->capacity = capacity;
        return;
    }

    self->data = alloc_resize(self->alloc, self->data, capacity);
    self->capacity = capacity;
}

void buffer_clear(Buffer *self)
{
    self->used = 0;
}

void buffer_push_impl(Buffer *self, uint8_t const *data, size_t size)
{
    buffer_ensure(self, self->used + size);

    mem_cpy(self->data + self->used, data, size);
    self->used += size;
}

static IoReadResult io_buffer_read_impl(IoFBufferReader *read, char *data, size_t size)
{
    size_t readed = MIN(size + read->cur, read->buf->used);

    for (size_t i = 0; i < readed; i++)
    {
        data[i] = read->buf->data[i + read->cur];
    }
    read->cur += readed;
    return OK(IoReadResult, readed);
}
IoFBufferReader io_buffer_read(Buffer const *self)
{
    IoFBufferReader reader = (IoFBufferReader){
        .base = {(IoRead *)io_buffer_read_impl},
        .buf = self,
    };
    return reader;
}

static IoWriteResult io_buffer_write_impl(IoBufferWriter *writer, char const *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        buffer_push(writer->buf, data[i]);
    }
    return OK(IoWriteResult, size);
}
IoBufferWriter io_buffer_write(Buffer *self)
{
    IoBufferWriter writer = (IoBufferWriter){
        .base = {(IoWrite *)io_buffer_write_impl},
        .buf = self,
    };
    return writer;
}
