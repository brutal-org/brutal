#include <brutal/base/attributes.h>
#include <brutal/base/macros.h>
#include <brutal/io/buf.h>

void buf_init(Buf *self, size_t capacity, Alloc *alloc)
{
    *self = (Buf){
        .data = nullptr,
        .used = 0,
        .capacity = 0,
        .alloc = alloc,
    };

    buf_ensure(self, capacity);
}

void buf_deinit(Buf *self)
{
    alloc_free(self->alloc, self->data);
    *self = (Buf){};
}

void buf_ensure(Buf *self, size_t capacity)
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

void buf_clear(Buf *self)
{
    self->used = 0;
}

void buf_push_impl(Buf *self, uint8_t const *data, size_t size)
{
    buf_ensure(self, self->used + size);

    mem_cpy(self->data + self->used, data, size);
    self->used += size;
}

Str buf_str(Buf *self)
{
    return str_n$(self->used, (char *)self->data);
}

static IoResult buf_read_impl(Buf *self, char *data, size_t offset, size_t size)
{
    size_t read = MIN(size, self->used - offset);

    for (size_t i = 0; i < read; i++)
    {
        data[i] = self->data[offset + i];
    }

    return OK(IoResult, read);
}

IoReader buf_reader(Buf *self)
{
    return (IoReader){
        .read = (IoRead *)buf_read_impl,
        .context = self,
    };
}

static IoResult buf_write_impl(Buf *self, char const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        buf_push(self, data[i]);
    }

    return OK(IoResult, size);
}

IoWriter buf_writer(Buf *self)
{
    return (IoWriter){
        .write = (IoWrite *)buf_write_impl,
        .context = self,
    };
}
