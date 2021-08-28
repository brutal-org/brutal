#pragma once

#include <brutal/base/attributes.h>
#include <brutal/gfx/color.h>
#include <brutal/log/assert.h>

typedef enum
{
    GFX_PIXEL_FORMAT_UNKNOWN,

    GFX_PIXEL_FORMAT_GRAYSCALE_8,
    GFX_PIXEL_FORMAT_RGB888,
    GFX_PIXEL_FORMAT_RGBA8888,
    GFX_PIXEL_FORMAT_BGRA8888,
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

typedef struct PACKED
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} GfxPixelBGRA8888;

static inline void gfx_pixel_store(GfxColor color, void *dst, GfxPixelFormat format)
{
    switch (format)
    {
    case GFX_PIXEL_FORMAT_GRAYSCALE_8:
    {
        GfxPixelGrayScale8 *p = (GfxPixelGrayScale8 *)dst;
        p->v = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    }
    break;

    case GFX_PIXEL_FORMAT_RGB888:
    {
        GfxPixelRGB888 *p = (GfxPixelRGB888 *)dst;
        p->r = color.r;
        p->g = color.g;
        p->b = color.b;
    }
    break;

    case GFX_PIXEL_FORMAT_RGBA8888:
    {
        GfxPixelRGBA8888 *p = (GfxPixelRGBA8888 *)dst;
        p->r = color.r;
        p->g = color.g;
        p->b = color.b;
        p->a = color.a;
    }
    break;

    case GFX_PIXEL_FORMAT_BGRA8888:
    {
        GfxPixelBGRA8888 *p = (GfxPixelBGRA8888 *)dst;
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
        GfxPixelGrayScale8 p = *((GfxPixelGrayScale8 const *)src);
        return (GfxColor){p.v, p.v, p.v, 0};
    }

    case GFX_PIXEL_FORMAT_RGB888:
    {
        GfxPixelRGB888 p = *((GfxPixelRGB888 const *)src);
        return (GfxColor){p.r, p.g, p.b, 0};
    }

    case GFX_PIXEL_FORMAT_RGBA8888:
    {
        GfxPixelRGBA8888 p = *((GfxPixelRGBA8888 const *)src);
        return (GfxColor){p.r, p.g, p.b, p.a};
    }

    case GFX_PIXEL_FORMAT_BGRA8888:
    {
        GfxPixelBGRA8888 p = *((GfxPixelBGRA8888 const *)src);
        return (GfxColor){p.r, p.g, p.b, p.a};
    }

    default:
        assert_unreachable();
    }
}

static inline size_t gfx_pixel_size(GfxPixelFormat format)
{
    switch (format)
    {
    case GFX_PIXEL_FORMAT_GRAYSCALE_8:
        return sizeof(GfxPixelGrayScale8);

    case GFX_PIXEL_FORMAT_RGB888:
        return sizeof(GfxPixelRGB888);

    case GFX_PIXEL_FORMAT_RGBA8888:
        return sizeof(GfxPixelRGBA8888);

    case GFX_PIXEL_FORMAT_BGRA8888:
        return sizeof(GfxPixelBGRA8888);

    default:
        assert_unreachable();
    }
}
