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
} GfxDynBuf;

void gfx_dyn_buf_init(GfxDynBuf *self, Alloc *alloc, int width, int height, GfxFmt format);

void gfx_dyn_buf_deinit(GfxDynBuf *self);

GfxBuf gfx_dyn_buf_buf(GfxDynBuf *self);

static inline GfxColor gfx_buf_load(GfxBuf self, int x, int y)
{
    if (x < 0 || x > self.width || y < 0 || y > self.height)
    {
        return GFX_MAGENTA;
    }

    uint8_t *pixel = ((uint8_t *)self.buf) + self.pitch * y + x * gfx_fmt_size(self.fmt);
    return gfx_fmt_load(self.fmt, pixel);
}

static inline void gfx_buf_store(GfxBuf self, int x, int y, GfxColor color)
{
    if (x < 0 || x > self.width || y < 0 || y > self.height)
    {
        return;
    }

    uint8_t *buf = (uint8_t *)self.buf;
    uint8_t *pixel = buf + self.pitch * y + x * gfx_fmt_size(self.fmt);
    gfx_fmt_store(self.fmt, color, pixel);
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
            gfx_buf_blend(dst, x + xx, y + yy, c);
        }
    }
}

static inline void gfx_buf_clear(GfxBuf self, GfxColor color)
{
    for (int y = 0; y < self.height; y++)
    {
        for (int x = 0; x < self.width; x++)
        {
            gfx_buf_store(self, x, y, color);
        }
    }
}

static inline MRect gfx_buf_bound(GfxBuf self)
{
    return m_rect(0, 0, self.width, self.height);
}
