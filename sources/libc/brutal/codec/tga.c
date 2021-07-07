#include <brutal/codec/tga.h>

WriteResult tga_encode(struct writer *writer, GfxSurface *surface)
{
    TgaHeader header = {
        .data_type_code = le_cast(le_int8_t, TGA_DATATYPE_UNCOMPRESSED_RGB),
        .width = le_cast(le_int16_t, surface->width),
        .height = le_cast(le_int16_t, surface->height),
        .bits_per_pixel = le_cast(le_int8_t, 32),
    };

    size_t written = 0;

    written = TRY(WriteResult, io_write(writer, &header, sizeof(TgaHeader)));
    written += TRY(WriteResult, io_write(writer, surface->buffer, surface->size));

    return OK(WriteResult, written);
}
