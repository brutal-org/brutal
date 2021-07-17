#pragma once

#include <brutal/alloc/base.h>
#include <brutal/gfx/color.h>
#include <brutal/gfx/format.h>
#include <brutal/math/rect.h>

typedef struct
{
    int width;
    int height;
    GfxPixelFormat format;
    void *buffer;
    size_t size;
} GfxSurface;

typedef struct
{
    GfxSurface surface;
    Alloc *alloc;
} GfxBuffer;

void gfx_buffer_init(GfxBuffer *self, Alloc *alloc, int width, int height, GfxPixelFormat format);

void gfx_buffer_deinit(GfxBuffer *self);

GfxSurface gfx_buffer_surface(GfxBuffer *self);

void gfx_surface_clear(GfxSurface surface, GfxColor color);

static inline Recti gfx_surface_bound(GfxSurface surface)
{
    return (Recti){0, 0, surface.width, surface.height};
}
