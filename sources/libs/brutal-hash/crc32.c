#include "crc32.h"

static uint32_t const crc32tab[16] = {
    0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190,
    0x6B6B51F4, 0x4DB26158, 0x5005713C, 0xEDB88320, 0xF00F9344,
    0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278,
    0xBDBDF21C};

void crc32_init_writer(Crc32 *self, IoWriter base)
{
    self->crc = 0xFFFFFFFF;
    self->count = 0;
    self->base_writer = base;
}

void crc32_init_reader(Crc32 *self, IoReader base)
{
    self->crc = 0xFFFFFFFF;
    self->count = 0;
    self->base_reader = base;
}

static uint32_t crc32_impl(uint32_t crc, uint8_t const *data, size_t size)
{
    for (size_t n = 0; n < size; ++n)
    {
        crc ^= data[n];
        crc = crc32tab[crc & 0x0F] ^ (crc >> 4);
        crc = crc32tab[crc & 0x0F] ^ (crc >> 4);
    }
    return crc;
}

static IoResult crc32_write_impl(Crc32 *self, uint8_t const *data, size_t size)
{
    self->crc = crc32_impl(self->crc, data, size);
    self->count += size;

    return io_write(self->base_writer, data, size);
}

IoWriter crc32_writer(Crc32 *self)
{
    return (IoWriter){
        .write = (IoWriteFn *)crc32_write_impl,
        .context = self,
    };
}

static IoResult crc32_read_impl(Crc32 *self, uint8_t *data, size_t size)
{
    size_t read = TRY(IoResult, io_read(self->base_reader, data, size));
    self->crc = crc32_impl(self->crc, data, read);
    self->count += read;

    return OK(IoResult, read);
}

IoReader crc32_reader(Crc32 *self)
{
    return (IoReader){
        .read = (IoReadFn *)crc32_read_impl,
        .context = self,
    };
}

uint32_t crc32_get(Crc32 *self)
{
    return self->crc ^ 0xFFFFFFFF;
}

size_t crc32_count(Crc32 *self)
{
    return self->count;
}

uint32_t crc32(void const *buf, size_t size)
{
    uint8_t *data = (uint8_t *)buf;
    uint32_t crc = crc32_impl(0xFFFFFFFF, data, size);

    return crc ^ 0xFFFFFFFF;
}
