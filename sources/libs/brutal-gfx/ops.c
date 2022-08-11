#include <string.h>

#include "ops.h"

/* --- gfx_ops_copy --------------------------------------------------------- */

void gfx_ops_copy_same_fmt(GfxBuf dst, GfxBuf src, int x, int y)
{
    for (int yy = 0; yy < src.height; yy++)
    {
        uint8_t *dst_pixels = ((uint8_t *)dst.buf) + dst.pitch * (yy + y);
        uint8_t const *src_pixels = ((uint8_t const *)src.buf) + src.pitch * yy;

        mem_cpy(dst_pixels + x, src_pixels, src.width * gfx_fmt_size(src.fmt));
    }
}

void gfx_ops_copy(GfxBuf dst, GfxBuf src, int x, int y)
{
    if (dst.fmt == src.fmt &&
        m_rectf_contains_rect(
            gfx_buf_bound(src),
            m_rectf_move(gfx_buf_bound(dst), m_vec2f(x, y))))
    {
        return gfx_ops_copy_same_fmt(dst, src, x, y);
    }

    for (int yy = 0; yy < src.height; yy++)
    {
        for (int xx = 0; xx < src.width; xx++)
        {
            GfxColor c = gfx_buf_load(src, xx, yy);
            gfx_buf_store(dst, x + xx, y + yy, c);
        }
    }
}

/* --- gfx_ops_flip --------------------------------------------------------- */

static void gfx_ops_flip_generic(GfxBuf dst, GfxBuf src, MRectf rect)
{
    for (int yy = rect.y; yy < rect.y + rect.height; yy++)
    {
        for (int xx = rect.x; xx < rect.x + rect.width; xx++)
        {
            GfxColor c = gfx_buf_load(src, xx, yy);
            gfx_buf_store(dst, xx, yy, c);
        }
    }
}

static void gfx_ops_flip_same_fmt(GfxBuf dst, GfxBuf src, MRectf rect)
{
    for (int yy = rect.y; yy < rect.y + rect.height; yy++)
    {
        uint8_t *dst_pixels = ((uint8_t *)dst.buf) + dst.pitch * yy;
        uint8_t *src_pixels = ((uint8_t *)src.buf) + src.pitch * yy;

        memcpy(dst_pixels, src_pixels, rect.width * gfx_fmt_size(dst.fmt));
    }
}

void gfx_ops_flip(GfxBuf dst, GfxBuf src, MRectf rect)
{
    if (dst.fmt == src.fmt)
    {
        gfx_ops_flip_same_fmt(dst, src, rect);
    }
    else
    {
        gfx_ops_flip_generic(dst, src, rect);
    }
}

/* --- gfx_ops_clear -------------------------------------------------------- */

static inline void gfx_ops_clear_common(GfxBuf self, GfxColor color, int const size)
{
    uint8_t pixel[64];
    gfx_fmt_store(self.fmt, color, &pixel);
    uint8_t *target = self.buf;

    for (int y = 0; y < self.height; y++)
    {
        for (int x = 0; x < self.width; x++)
        {
            uint8_t *t = target + self.pitch * y + x * size;
            memcpy(t, pixel, size);
        }
    }
}

FLATTEN void gfx_ops_clear(GfxBuf self, GfxColor color)
{
    // This code might look redundant, but it let the compiler know the pixel size at compile time.

    size_t size = gfx_fmt_size(self.fmt);

    switch (size)
    {
    case 1:
        gfx_ops_clear_common(self, color, 1);
        break;

    case 2:
        gfx_ops_clear_common(self, color, 2);
        break;

    case 4:
        gfx_ops_clear_common(self, color, 4);
        break;

    case 8:
        gfx_ops_clear_common(self, color, 8);
        break;

    default:
        assert_lower_equal(size, 64u);
        gfx_ops_clear_common(self, color, size);
        break;
    }
}

/* --- gfx_ops_rect --------------------------------------------------------- */

static inline void gfx_ops_rect_common(GfxBuf self, MRectf rect, GfxColor color, int const size)
{
    uint8_t pixel[64];
    gfx_fmt_store(self.fmt, color, &pixel);
    uint8_t *target = self.buf;

    for (int y = rect.y; y < rect.y + rect.height; y++)
    {
        for (int x = rect.x; x < rect.x + rect.width; x++)
        {
            uint8_t *t = target + self.pitch * y + x * size;
            memcpy(t, pixel, size);
        }
    }
}

FLATTEN void gfx_ops_rect(GfxBuf self, MRectf rect, GfxColor color)
{
    // This code might look redundant, but it let the compiler know the pixel size at compile time.

    size_t size = gfx_fmt_size(self.fmt);

    switch (size)
    {
    case 1:
        gfx_ops_rect_common(self, rect, color, 1);
        break;

    case 2:
        gfx_ops_rect_common(self, rect, color, 2);
        break;

    case 4:
        gfx_ops_rect_common(self, rect, color, 4);
        break;

    case 8:
        gfx_ops_rect_common(self, rect, color, 8);
        break;

    default:
        assert_lower_equal(size, 64u);
        gfx_ops_rect_common(self, rect, color, size);
        break;
    }
}
