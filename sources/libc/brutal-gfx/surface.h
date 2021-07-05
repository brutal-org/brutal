#pragma once

#include <brutal-gfx/color.h>

typedef struct
{
    int width;
    int height;

    GfxColor *buffer;
} GfxSurface;

void gfx_surface_clear(GfxSurface surface, GfxColor color);
