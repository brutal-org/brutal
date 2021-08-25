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
        self->data = (uint8_t *)alloc_calloc(self->alloc, 1, capacity);
        self->capacity = capacity;
        return;
    }

    self->data = (uint8_t *)alloc_resize(self->alloc, self->data, capacity);
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

Str buffer_str(Buffer *self)
{
    return str_n$(self->used, (char *)self->data);
}

static IoReadResult buffer_read_impl(Buffer *self, char *data, size_t offset, size_t size)
{
    size_t read = MIN(size, self->used - offset);

    for (size_t i = 0; i < read; i++)
    {
        data[i] = self->data[offset + i];
    }

    return OK(IoReadResult, read);
}

IoReader buffer_reader(Buffer *self)
{
    return (IoReader){
        .read = (IoRead *)buffer_read_impl,
        .context = self,
    };
}

static IoWriteResult buffer_write_impl(Buffer *self, char const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        buffer_push(self, data[i]);
    }

    return OK(IoWriteResult, size);
}

IoWriter buffer_writer(Buffer *self)
{
    return (IoWriter){
        .write = (IoWrite *)buffer_write_impl,
        .context = self,
    };
}
