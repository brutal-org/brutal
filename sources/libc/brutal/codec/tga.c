#include <brutal/codec/tga.h>
#include "brutal/base/endian.h"

WriteResult tga_encode(struct writer *writer, GfxSurface *surface)
{
    TgaHeader header = (TgaHeader){
        .data_type_code = le_cast(le_int8_t, TGA_DATATYPE_UNCOMPRESSED_RGB),
        .width = le_cast(le_int16_t, surface->width),
        .height = le_cast(le_int16_t, surface->height),
        .bits_per_pixel = le_cast(le_int8_t, 32),
        .image_descriptor = le_cast(le_int8_t, 0), // by default don't use an alpha channel
    };

    if (surface->format == GFX_PIXEL_FORMAT_RGBA8888) // set the number of bit for the alpha channel
    {
        header.image_descriptor = le_cast(le_int8_t, 8 & TGA_IMGDESC_ALPHA_BITS);
    }

    size_t written = 0;

    written = TRY(WriteResult, io_write(writer, &header, sizeof(TgaHeader)));
    written += TRY(WriteResult, io_write(writer, surface->buffer, surface->size));

    return OK(WriteResult, written);
}
