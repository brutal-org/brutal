#include <brutal-debug>
#include <brutal-math>

#include "buf.h"

void buf_init(Buf *self, size_t capacity, Alloc *alloc)
{
    *self = (Buf){
        .data = nullptr,
        .used = 0,
        .pos = 0,
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

static IoResult buf_read_impl(void *ctx, uint8_t *data, size_t size)
{
    Buf *self = (Buf *)ctx;

    size_t read = m_min(size, self->used - self->pos);

    for (size_t i = 0; i < read; i++)
    {
        data[i] = self->data[self->pos++];
    }

    return OK(IoResult, read);
}

IoReader buf_reader(Buf *self)
{
    return (IoReader){
        .read = buf_read_impl,
        .context = self,
    };
}

static IoResult buf_write_impl(void *ctx, uint8_t const *data, size_t size)
{
    Buf *self = (Buf *)ctx;
    buf_push_impl(self, data, size);
    return OK(IoResult, size);
}

IoWriter buf_writer(Buf *self)
{
    return (IoWriter){
        .write = buf_write_impl,
        .context = self,
    };
}

IoDuplex buf_duplex(Buf *self)
{
    return (IoDuplex){
        .read = buf_read_impl,
        .write = buf_write_impl,
        .context = self,
    };
}

IoResult buf_seek_impl(void *ctx, IoSeek seek)
{
    Buf *self = (Buf *)ctx;
    self->pos = io_seek_eval(seek, self->pos, self->used);
    return OK(IoResult, self->pos);
}

IoSeeker buf_seeker(Buf *self)
{
    return (IoSeeker){
        .seek = (IoSeekFn *)buf_seek_impl,
        .context = self,
    };
}

IoRSeek buf_rseek(Buf *self)
{
    return (IoRSeek){
        .read = buf_read_impl,
        .seek = buf_seek_impl,
        .context = self,
    };
}
