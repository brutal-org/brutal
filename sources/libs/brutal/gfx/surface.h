#pragma once

#include <brutal/alloc/base.h>
#include <brutal/gfx/color.h>
#include <brutal/gfx/format.h>
#include <brutal/math/rect.h>

typedef struct
{
    int width;
    int height;
    int pitch;
    GfxPixelFormat format;
    void *buf;
    size_t size;
} GfxSurface;

typedef struct
{
    GfxSurface surface;
    Alloc *alloc;
} GfxBuf;

void gfx_buf_init(GfxBuf *self, Alloc *alloc, int width, int height, GfxPixelFormat format);

void gfx_buf_deinit(GfxBuf *self);

GfxSurface gfx_buf_surface(GfxBuf *self);

static inline GfxColor gfx_surface_load(GfxSurface self, int x, int y)
{
    if (x < 0 || x > self.width || y < 0 || y > self.height)
    {
        return GFX_COLOR_MAGENTA;
    }

    uint8_t *pixel = ((uint8_t *)self.buf) + self.pitch * y + x * gfx_pixel_size(self.format);
    return gfx_pixel_load(pixel, self.format);
}

static inline void gfx_surface_store(GfxSurface self, int x, int y, GfxColor color)
{
    if (x < 0 || x > self.width || y < 0 || y > self.height)
    {
        return;
    }

    uint8_t *buf = (uint8_t *)self.buf;
    uint8_t *pixel = buf + self.pitch * y + x * gfx_pixel_size(self.format);
    gfx_pixel_store(color, pixel, self.format);
}

static inline void gfx_surface_blend(GfxSurface self, int x, int y, GfxColor color)
{
    GfxColor bg = gfx_surface_load(self, x, y);
    GfxColor blend = gfx_color_blend(color, bg);
    gfx_surface_store(self, x, y, blend);
}

static inline void gfx_surface_copy(GfxSurface dst, GfxSurface src, int x, int y)
{
    for (int yy = 0; yy < src.height; yy++)
    {
        for (int xx = 0; xx < src.width; xx++)
        {
            GfxColor c = gfx_surface_load(src, xx, yy);
            gfx_surface_blend(dst, x + xx, y + yy, c);
        }
    }
}

static inline void gfx_surface_clear(GfxSurface self, GfxColor color)
{
    for (int y = 0; y < self.height; y++)
    {
        for (int x = 0; x < self.width; x++)
        {
            gfx_surface_store(self, x, y, color);
        }
    }
}

static inline Recti gfx_surface_bound(GfxSurface self)
{
    return (Recti){0, 0, self.width, self.height};
}
