#pragma once

#include <brutal-gfx/buf.h>
#include <brutal-io>

// using http://tfc.duke.free.fr/coding/tga_specs.pdf

#define TGA_IMGDESC_ORIGIN_BITS 0b00110000
#define TGA_IMGDESC_ALPHA_BITS 0b00001111

enum tga_image_origin
{
    TGA_BOTTOM_LEFT = 0b00000000,
    TGA_BOTTOM_RIGHT = 0b00010000,
    TGA_TOP_LEFT = 0b00100000,
    TGA_TOP_RIGHT = 0b00110000,
};

typedef struct PACKED
{
    le_int8_t id_len;
    le_int8_t colour_map_type;

#define TGA_DATATYPE_UNCOMPRESSED_RGB (2)
    le_int8_t data_type_code;
    le_int16_t colour_map_origin;
    le_int16_t colour_map_len;
    le_int8_t colour_map_depth;
    le_int16_t x_origin;
    le_int16_t y_origin;
    le_int16_t width;
    le_int16_t height;
    le_int8_t bits_per_pixel;
    le_int8_t image_descriptor;
} TgaHeader;

IoResult tga_encode(IoWriter writer, GfxBuf surface);

GfxBuf tga_decode_in_memory(void *addr, size_t size);
