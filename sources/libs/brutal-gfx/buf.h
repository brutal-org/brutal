#pragma once

#include <brutal-alloc>
#include <brutal-math>

#include "color.h"
#include "fmt.h"

typedef struct
{
    int width;
    int height;
    int pitch;
    GfxFmt fmt;
    void *buf;
} GfxBuf;

typedef struct
{
    GfxBuf buf;
    Alloc *alloc;
} GfxSurface;

void gfx_surface_init(GfxSurface *self, int width, int height, GfxFmt format, Alloc *alloc);

void gfx_surface_deinit(GfxSurface *self);

GfxBuf gfx_surface_buf(GfxSurface *self);

static inline GfxColor gfx_buf_load_uncheck(const GfxBuf self, int x, int y)
{
    const uint8_t *pixel = ((const uint8_t *)self.buf) + self.pitch * y + x * gfx_fmt_size(self.fmt);
    return gfx_fmt_load(self.fmt, pixel);
}

static inline void gfx_buf_store_unckeck(GfxBuf self, int x, int y, GfxColor color)
{
    uint8_t *pixel = ((uint8_t *)self.buf) + self.pitch * y + x * gfx_fmt_size(self.fmt);
    gfx_fmt_store(self.fmt, color, pixel);
}

static inline GfxColor gfx_buf_load(GfxBuf self, int x, int y)
{
    if (UNLIKELY(x < 0 || x >= self.width || y < 0 || y >= self.height))
    {
        return GFX_MAGENTA;
    }

    return gfx_buf_load_uncheck(self, x, y);
}

static inline void gfx_buf_store(GfxBuf self, int x, int y, GfxColor color)
{
    if (UNLIKELY(x < 0 || x >= self.width || y < 0 || y >= self.height))
    {
        return;
    }

    gfx_buf_store_unckeck(self, x, y, color);
}

static inline MRectf gfx_buf_bound(GfxBuf self)
{
    return m_rectf(0, 0, self.width, self.height);
}

static inline size_t gfx_buf_size(GfxBuf self)
{
    return self.width * self.height * gfx_fmt_size(self.fmt);
}

static inline GfxColor gfx_buf_sample(GfxBuf self, float x, float y)
{
    int xi = (int)x;
    int yi = (int)y;
    return gfx_buf_load(self, xi, yi);
}

static inline void gfx_buf_blend_unckeck(GfxBuf self, int x, int y, GfxColor color)
{
    if (color.a == 0xff)
    {
        return gfx_buf_store(self, x, y, color);
    }

    GfxColor bg = gfx_buf_load_uncheck(self, x, y);
    GfxColor blend = gfx_blend(color, bg);
    gfx_buf_store_unckeck(self, x, y, blend);
}

static inline void gfx_buf_blend(GfxBuf self, int x, int y, GfxColor color)
{
    if (color.a == 0xff)
    {
        return gfx_buf_store(self, x, y, color);
    }
    GfxColor bg = gfx_buf_load(self, x, y);
    GfxColor blend = gfx_blend(color, bg);

    gfx_buf_store(self, x, y, blend);
}
