#include <brutal/base/attributes.h>
#include <brutal/base/macros.h>
#include <brutal/io/mem_view.h>
#include <brutal/math/clamp.h>

void mem_view_init(MemView *self, size_t capacity, const void *data)
{
    *self = (MemView){
        .data = (uint8_t*)data,
        .used = 0,
        .capacity = capacity,
    };
}

static IoResult mem_view_read_impl(MemView *self, char *data, size_t size)
{
    size_t read = m_min(size, self->capacity - self->used);

    for (size_t i = 0; i < read; i++)
    {
        data[i] = self->data[self->used + i];
    }

    self->used += read;

    return OK(IoResult, read);
}

IoReader mem_view_reader(MemView *self)
{
    return (IoReader){
        .read = (IoReadFn *)mem_view_read_impl,
        .context = self,
    };
}

static IoResult mem_view_write_impl(MemView *self, char const *data, size_t size)
{
    size_t to_write = m_min(self->capacity - self->used, size);
    mem_cpy(self->data + self->used, data, to_write);
    self->used += to_write;
    return OK(IoResult, to_write);
}

IoWriter mem_view_writer(MemView *self)
{
    return (IoWriter){
        .write = (IoWriteFn *)mem_view_write_impl,
        .context = self,
    };
}
