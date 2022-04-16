#pragma once

#include <brutal-debug>

#include "color.h"

typedef enum
{
    GFX_FMT_UNKNOWN,

    GFX_FMT_GS8,
    GFX_FMT_RGB888,
    GFX_FMT_RGBA8888,
    GFX_FMT_BGRA8888,
} GfxFmt;

typedef struct PACKED
{
    uint8_t v;
} GfxFmtGS8;

typedef struct PACKED
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} GfxFmtRGB888;

typedef struct PACKED
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} GfxFmtRGBA8888;

typedef struct PACKED
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} GfxFmtBGRA8888;

static inline void gfx_fmt_store(GfxFmt self, GfxColor color, void *dst)
{
    switch (self)
    {
    case GFX_FMT_GS8:
    {
        GfxFmtGS8 *p = (GfxFmtGS8 *)dst;
        p->v = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
        break;
    }

    case GFX_FMT_RGB888:
    {
        GfxFmtRGB888 *p = (GfxFmtRGB888 *)dst;
        p->r = color.r;
        p->g = color.g;
        p->b = color.b;
        break;
    }

    case GFX_FMT_RGBA8888:
    {
        GfxFmtRGBA8888 *p = (GfxFmtRGBA8888 *)dst;
        p->r = color.r;
        p->g = color.g;
        p->b = color.b;
        p->a = color.a;
        break;
    }

    case GFX_FMT_BGRA8888:
    {
        GfxFmtBGRA8888 *p = (GfxFmtBGRA8888 *)dst;
        p->r = color.r;
        p->g = color.g;
        p->b = color.b;
        p->a = color.a;
        break;
    }

    default:
        panic$("Unknown pixel format {}", self);
    }
}

static inline GfxColor gfx_fmt_load(GfxFmt self, const void *src)
{
    switch (self)
    {
    case GFX_FMT_GS8:
    {
        GfxFmtGS8 p = *((GfxFmtGS8 const *)src);
        return gfx_rgba(p.v, p.v, p.v, 0);
    }

    case GFX_FMT_RGB888:
    {
        GfxFmtRGB888 p = *((GfxFmtRGB888 const *)src);
        return gfx_rgba(p.r, p.g, p.b, 0);
    }

    case GFX_FMT_RGBA8888:
    {
        GfxFmtRGBA8888 p = *((GfxFmtRGBA8888 const *)src);
        return gfx_rgba(p.r, p.g, p.b, p.a);
    }

    case GFX_FMT_BGRA8888:
    {
        GfxFmtBGRA8888 p = *((GfxFmtBGRA8888 const *)src);
        return gfx_rgba(p.r, p.g, p.b, p.a);
    }

    default:
        panic$("Unknown pixel format {}", self);
    }
}

static inline size_t gfx_fmt_size(GfxFmt self)
{
    switch (self)
    {
    case GFX_FMT_GS8:
        return sizeof(GfxFmtGS8);

    case GFX_FMT_RGB888:
        return sizeof(GfxFmtRGB888);

    case GFX_FMT_RGBA8888:
        return sizeof(GfxFmtRGBA8888);

    case GFX_FMT_BGRA8888:
        return sizeof(GfxFmtBGRA8888);

    default:
        panic$("Unknown pixel format {}", self);
    }
}
