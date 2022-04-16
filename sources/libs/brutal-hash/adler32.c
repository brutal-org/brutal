#include "adler32.h"

#define A32_BASE 65521

void adler32_init(Adler32 *self, IoWriter underlying)
{
    self->s1 = 1;
    self->s2 = 0;
    self->underlying = underlying;
}

static IoResult adler32_write_impl(Adler32 *self, uint8_t const *data, size_t size)
{
    for (size_t n = 0; n < size; n++)
    {
        self->s1 = (self->s1 + data[n]) % A32_BASE;
        self->s2 = (self->s2 + self->s1) % A32_BASE;
    }
    return io_write(self->underlying, data, size);
}

IoWriter adler32_writer(Adler32 *self)
{
    return (IoWriter){
        .write = (IoWriteFn *)adler32_write_impl,
        .context = self,
    };
}

uint32_t adler32_get(Adler32 *self)
{
    return (self->s2 << 16) | self->s1;
}

uint32_t adler32(void const *buf, size_t len)
{
    uint8_t *bp = (uint8_t *)buf;
    uint32_t s1 = 1;
    uint32_t s2 = 0;

    for (size_t n = 0; n < len; n++)
    {
        s1 = (s1 + bp[n]) % A32_BASE;
        s2 = (s2 + s1) % A32_BASE;
    }

    return (s2 << 16) | s1;
}
