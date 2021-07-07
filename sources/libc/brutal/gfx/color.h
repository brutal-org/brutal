#pragma once

#include <brutal/base/std.h>

typedef struct
{
    uint8_t r, g, b, a;
} GfxColor;

static_assert(sizeof(GfxColor) == sizeof(uint32_t), "GfxColor should fit in an uint32_t");
