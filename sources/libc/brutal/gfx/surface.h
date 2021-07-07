#pragma once

#include <brutal/gfx/color.h>
#include <brutal/gfx/format.h>

typedef struct
{
} GfxSurfaceBuffer;

typedef struct
{
    int width;
    int height;
    GfxPixelFormat format;

    void *buffer;
    size_t size;
} GfxSurface;

void gfx_surface_clear(GfxSurface surface, GfxColor color);
