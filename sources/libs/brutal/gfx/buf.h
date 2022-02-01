#pragma once

#include <brutal/alloc/base.h>
#include <brutal/gfx/color.h>
#include <brutal/gfx/fmt.h>
#include <brutal/math/rect.h>

typedef struct
{
    int width;
    int height;
    int pitch;
    GfxFmt fmt;

    void *buf;
    size_t size;
} GfxBuf;

typedef struct
{
    GfxBuf buf;
    Alloc *alloc;
} GfxSurface;

void gfx_surface_init(GfxSurface *self, int width, int height, GfxFmt format, Alloc *alloc);

void gfx_surface_deinit(GfxSurface *self);

GfxBuf gfx_surface_buf(GfxSurface *self);

static inline GfxColor gfx_buf_load_uncheck(GfxBuf self, int x, int y)
{
    uint8_t *pixel = ((uint8_t *)self.buf) + self.pitch * y + x * gfx_fmt_size(self.fmt);
    return gfx_fmt_load(self.fmt, pixel);
}

static inline void gfx_buf_store_unckeck(GfxBuf self, int x, int y, GfxColor color)
{
    uint8_t *pixel = ((uint8_t *)self.buf)  + self.pitch * y + x * gfx_fmt_size(self.fmt);
    gfx_fmt_store(self.fmt, color, pixel);
}

static inline GfxColor gfx_buf_load(GfxBuf self, int x, int y)
{
    if (x < 0 || x > self.width || y < 0 || y > self.height)
    {
        return GFX_MAGENTA;
    }

    return gfx_buf_load_uncheck(self, x, y);
}

static inline void gfx_buf_store(GfxBuf self, int x, int y, GfxColor color)
{
    if (x < 0 || x > self.width || y < 0 || y > self.height)
    {
        return;
    }

    gfx_buf_store_unckeck(self, x, y, color);
}

static inline void gfx_buf_blend_unckeck(GfxBuf self, int x, int y, GfxColor color)
{
    GfxColor bg = gfx_buf_load_uncheck(self, x, y);
    GfxColor blend = gfx_blend(color, bg);
    gfx_buf_store_unckeck(self, x, y, blend);
}

static inline void gfx_buf_blend(GfxBuf self, int x, int y, GfxColor color)
{
    GfxColor bg = gfx_buf_load(self, x, y);
    GfxColor blend = gfx_blend(color, bg);
    gfx_buf_store(self, x, y, blend);
}

static inline void gfx_buf_copy(GfxBuf dst, GfxBuf src, int x, int y)
{
    for (int yy = 0; yy < src.height; yy++)
    {
        for (int xx = 0; xx < src.width; xx++)
        {
            GfxColor c = gfx_buf_load(src, xx, yy);
            gfx_buf_store(dst, x + xx, y + yy, c);
        }
    }
}

static inline void gfx_buf_clear(GfxBuf self, GfxColor color)
{
    for (int y = 0; y < self.height; y++)
    {
        for (int x = 0; x < self.width; x++)
        {
            gfx_buf_store_unckeck(self, x, y, color);
        }
    }
}

static inline MRect gfx_buf_bound(GfxBuf self)
{
    return m_rect(0, 0, self.width, self.height);
}
