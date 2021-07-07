#pragma once

#include <brutal/gfx/color.h>
#include <brutal/log/assert.h>

typedef enum
{
    GFX_PIXEL_FORMAT_UNKNOWN,

    GFX_PIXEL_FORMAT_GRAYSCALE_8,
    GFX_PIXEL_FORMAT_RGB888,
    GFX_PIXEL_FORMAT_RGBA8888,
} GfxPixelFormat;

typedef struct PACKED
{
    uint8_t v;
} GfxPixelGrayScale8;

typedef struct PACKED
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} GfxPixelRGB888;

typedef struct PACKED
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} GfxPixelRGBA8888;

static inline void gfx_pixel_store(GfxColor color, void *dst, GfxPixelFormat format)
{
    switch (format)
    {
    case GFX_PIXEL_FORMAT_GRAYSCALE_8:
    {
        auto p = (GfxPixelGrayScale8 *)dst;
        p->v = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    }
    break;

    case GFX_PIXEL_FORMAT_RGB888:
    {
        auto p = (GfxPixelRGB888 *)dst;
        p->r = color.r;
        p->g = color.g;
        p->b = color.b;
    }
    break;

    case GFX_PIXEL_FORMAT_RGBA8888:
    {
        auto p = (GfxPixelRGBA8888 *)dst;
        p->r = color.r;
        p->g = color.g;
        p->b = color.b;
        p->a = color.a;
    }
    break;

    default:
        assert_unreachable();
    }
}

static inline GfxColor gfx_pixel_load(const void *src, GfxPixelFormat format)
{
    switch (format)
    {
    case GFX_PIXEL_FORMAT_GRAYSCALE_8:
    {
        auto p = *((const GfxPixelGrayScale8 *)src);
        return (GfxColor){p.v, p.v, p.v, 0};
    }

    case GFX_PIXEL_FORMAT_RGB888:
    {
        auto p = *((const GfxPixelRGB888 *)src);
        return (GfxColor){p.r, p.g, p.b, 0};
    }

    case GFX_PIXEL_FORMAT_RGBA8888:
    {
        auto p = *((const GfxPixelRGBA8888 *)src);
        return (GfxColor){p.r, p.g, p.b, p.a};
    }

    default:
        assert_unreachable();
    }
}
