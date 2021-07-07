#pragma once

#include <brutal/base/endian.h>
#include <brutal/gfx/surface.h>
#include <brutal/io/write.h>

typedef struct
{
    le_int8_t id_length;
    le_int8_t colour_map_type;

#define TGA_DATATYPE_UNCOMPRESSED_RGB (2)
    le_int8_t data_type_code;
    le_int16_t colour_map_origin;
    le_int16_t colour_map_length;
    le_int8_t colour_map_depth;
    le_int16_t x_origin;
    le_int16_t y_origin;
    le_int16_t width;
    le_int16_t height;
    le_int8_t bits_per_pixel;
    le_int8_t image_descriptor;
} TgaHeader;

WriteResult tga_encode(struct writer *writer, GfxSurface *surface);
