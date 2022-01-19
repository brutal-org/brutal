#include <brutal/hash/crc32.h>
#include <brutal/io/funcs.h>

static const uint32_t crc32tab[16] = {
    0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190,
    0x6B6B51F4, 0x4DB26158, 0x5005713C, 0xEDB88320, 0xF00F9344,
    0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278,
    0xBDBDF21C};

void crc32_init(Crc32 *self, IoWriter underlying)
{
    self->crc = 0xFFFFFFFF;
    self->underlying = underlying;
}

static IoResult crc32_write_impl(Crc32 *self, uint8_t const *data, size_t size)
{
    for (size_t n = 0; n < size; ++n)
    {
        self->crc ^= data[n];
        self->crc = crc32tab[self->crc & 0x0F] ^ (self->crc >> 4);
        self->crc = crc32tab[self->crc & 0x0F] ^ (self->crc >> 4);
    }

    return io_write(self->underlying, data, size);
}

IoWriter crc32_writer(Crc32 *self)
{
    return (IoWriter){
        .write = (IoWriteFn *)crc32_write_impl,
        .context = self,
    };
}

uint32_t crc32_get(Crc32 *self)
{
    return self->crc ^ 0xFFFFFFFF;
}

uint32_t crc32(void const *buf, size_t len)
{
    uint8_t *bp = (uint8_t *)buf;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t n = 0; n < len; ++n)
    {
        crc ^= bp[n];
        crc = crc32tab[crc & 0x0F] ^ (crc >> 4);
        crc = crc32tab[crc & 0x0F] ^ (crc >> 4);
    }

    return crc ^ 0xFFFFFFFF;
}
