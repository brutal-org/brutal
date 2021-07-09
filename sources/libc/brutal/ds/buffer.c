#include <brutal/ds/buffer.h>

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
