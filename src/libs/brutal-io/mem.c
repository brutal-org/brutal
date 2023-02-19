#include <brutal-math>

#include "mem.h"

void io_mem_init(IoMem *self, size_t capacity, const void *data)
{
    *self = (IoMem){
        .data = (uint8_t *)data,
        .used = 0,
        .capacity = capacity,
    };
}

static IoResult io_mem_read(void *ctx, uint8_t *data, size_t size)
{
    IoMem *self = ctx;
    size_t read = m_min(size, self->capacity - self->used);

    for (size_t i = 0; i < read; i++)
    {
        data[i] = self->data[self->used + i];
    }

    self->used += read;

    return OK(IoResult, read);
}

IoReader io_mem_reader(IoMem *self)
{
    return (IoReader){
        .read = io_mem_read,
        .context = self,
    };
}

static IoResult io_mem_write_impl(IoMem *self, uint8_t const *data, size_t size)
{
    size_t to_write = m_min(self->capacity - self->used, size);
    mem_cpy(self->data + self->used, data, to_write);
    self->used += to_write;
    return OK(IoResult, to_write);
}

IoWriter io_mem_writer(IoMem *self)
{
    return (IoWriter){
        .write = (IoWriteFn *)io_mem_write_impl,
        .context = self,
    };
}
