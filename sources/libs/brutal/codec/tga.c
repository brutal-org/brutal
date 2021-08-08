#include <brutal/codec/tga.h>
#include "brutal/base/endian.h"

IoWriteResult tga_encode(IoWriter *writer, GfxSurface surface)
{
    TgaHeader header = (TgaHeader){
        .data_type_code = le_cast(le_int8_t, TGA_DATATYPE_UNCOMPRESSED_RGB),
        .width = le_cast(le_int16_t, surface.width),
        .height = le_cast(le_int16_t, surface.height),
        .bits_per_pixel = le_cast(le_int8_t, 32),
        .image_descriptor = le_cast(le_int8_t, 0), // by default don't use an alpha channel
    };

    if (surface.format == GFX_PIXEL_FORMAT_RGBA8888) // set the number of bit for the alpha channel
    {
        header.image_descriptor = le_cast(le_int8_t, TGA_IMGDESC_ALPHA_BITS);
    }

    size_t written = 0;

    written = TRY(IoWriteResult, io_write(writer, &header, sizeof(TgaHeader)));
    written += TRY(IoWriteResult, io_write(writer, surface.buffer, surface.size));

    return OK(IoWriteResult, written);
}

GfxSurface tga_decode_in_memory(void *addr, size_t size)
{
    //assert_greater_than(size, sizeof(TgaHeader));

    TgaHeader *header = addr;
    void *buffer = header + 1;

    return (GfxSurface){
        .width = load_le(header->width),
        .height = load_le(header->height),
        .pitch = load_le(header->width) * (load_le(header->bits_per_pixel) / 8),
        .format = GFX_PIXEL_FORMAT_RGBA8888,
        .buffer = buffer,
        .size = size - sizeof(TgaHeader),
    };
}
