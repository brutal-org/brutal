#include "tga.h"

IoResult tga_encode(IoWriter writer, GfxBuf surface)
{
    TgaHeader header = (TgaHeader){
        .data_type_code = le$(le_int8_t, TGA_DATATYPE_UNCOMPRESSED_RGB),
        .width = le$(le_int16_t, surface.width),
        .height = le$(le_int16_t, surface.height),
        .bits_per_pixel = le$(le_int8_t, 32),
        .image_descriptor = le$(le_int8_t, 0), // by default don't use an alpha channel
    };

    if (surface.fmt == GFX_FMT_RGBA8888) // set the number of bit for the alpha channel
    {
        header.image_descriptor = le$(le_int8_t, TGA_IMGDESC_ALPHA_BITS);
    }

    size_t written = 0;

    written = TRY(IoResult, io_write(writer, (uint8_t *)&header, sizeof(TgaHeader)));
    written += TRY(IoResult, io_write(writer, (uint8_t *)surface.buf, gfx_buf_size(surface)));

    return OK(IoResult, written);
}

GfxBuf tga_decode_in_memory(void *addr, size_t size)
{
    assert_greater_than(size, sizeof(TgaHeader));

    TgaHeader *header = (TgaHeader *)addr;
    void *buf = header + 1;

    return (GfxBuf){
        .width = le_load$(header->width),
        .height = le_load$(header->height),
        .pitch = le_load$(header->width) * (le_load$(header->bits_per_pixel) / 8),
        .fmt = GFX_FMT_RGBA8888,
        .buf = buf,
    };
}
