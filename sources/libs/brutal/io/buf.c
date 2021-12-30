#include <brutal/base/attributes.h>
#include <brutal/base/macros.h>
#include <brutal/io/buf.h>
#include <brutal/math/clamp.h>

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
    capacity = m_max(capacity, 16);

    if (capacity <= self->capacity)
        return;

    self->capacity = m_max(self->capacity, 16);

    while (self->capacity < capacity)
        self->capacity += self->capacity / 4;

    if (self->capacity == 0)
    {
        self->data = (uint8_t *)alloc_calloc(self->alloc, 1, self->capacity);
        return;
    }

    self->data = (uint8_t *)alloc_resize(self->alloc, self->data, self->capacity);
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
    size_t read = m_min(size, self->used - offset);

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
    buf_push_impl(self, (uint8_t const *)data, size);
    return OK(IoResult, size);
}

IoWriter buf_writer(Buf *self)
{
    return (IoWriter){
        .write = (IoWrite *)buf_write_impl,
        .context = self,
    };
}
