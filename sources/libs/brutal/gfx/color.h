#pragma once

#include <brutal/base/keywords.h>
#include <brutal/base/std.h>
#include <math.h>

typedef struct
{
    uint8_t r, g, b, a;
} GfxColor;

typedef struct
{
    float r, g, b, a;
} GfxColorf;

#define gfx_colorf$(COLOR) \
    ((GfxColorf){(COLOR).r / 255.0f, (COLOR).g / 255.0f, (COLOR).b / 255.0f, (COLOR).a / 255.0f})

#define gfx_colori$(COLORF) \
    ((GfxColor){(COLORF).r * 255, (COLORF).g * 255, (COLORF).b * 255, (COLORF).a * 255})

#define gfx_rgba(T, R, G, B, A) \
    ((T){.r = (R), .g = (G), .b = (B), .a = (A)})

#define gfx_rgb(T, r, g, b) \
    gfx_rgba(T, r, g, b, 0xff)

#define gfx_hex(hex) \
    gfx_rgba(GfxColor, (uint8_t)(hex >> 16), (uint8_t)(hex >> 8)), (uint8_t) hex, 0xff)

#define GFX_COLOR_WHITE \
    gfx_rgb(GfxColor, 0xff, 0xff, 0xff)

#define GFX_COLOR_BLACK \
    gfx_rgb(GfxColor, 0x00, 0x00, 0x00)

#define GFX_COLOR_RED \
    gfx_rgb(GfxColor, 0xff, 0x00, 0x00)

#define GFX_COLOR_GREEN \
    gfx_rgb(GfxColor, 0x00, 0xff, 0x00)

#define GFX_COLOR_BLUE \
    gfx_rgb(GfxColor, 0x00, 0x00, 0xff)

#define GFX_COLOR_MAGENTA \
    gfx_rgb(GfxColor, 0xff, 0x00, 0xff)

static_assert(sizeof(GfxColor) == sizeof(uint32_t), "GfxColor should fit in an uint32_t");

static inline GfxColor gfx_color_blend(GfxColor fg, GfxColor bg)
{
    if (fg.a == 0xff)
    {
        return fg;
    }

    if (fg.a == 0)
    {
        return bg;
    }

    if (bg.a == 0xff)
    {

        unsigned alpha = fg.a;
        unsigned inv_alpha = 255 - fg.a + 1;

        uint8_t r = (uint8_t)((alpha * fg.r + inv_alpha * bg.r) / 256);
        uint8_t g = (uint8_t)((alpha * fg.g + inv_alpha * bg.g) / 256);
        uint8_t b = (uint8_t)((alpha * fg.b + inv_alpha * bg.b) / 256);

        return gfx_rgba(GfxColor, r, g, b, 0xff);
    }

    GfxColorf fgf = gfx_colorf$(fg);
    GfxColorf bgf = gfx_colorf$(fg);

    float fgf_bgf_factor = (1 - fgf.a) * bgf.a;
    float a = fgf_bgf_factor + fgf.a;

    float r = (fgf_bgf_factor * bgf.r + fgf.a * fgf.r) / a;
    float g = (fgf_bgf_factor * bgf.g + fgf.a * fgf.g) / a;
    float b = (fgf_bgf_factor * bgf.b + fgf.a * fgf.b) / a;

    GfxColorf res = (GfxColorf){r, g, b, a};
    return gfx_colori$(res);
}

static inline GfxColorf gfx_color_gamma_correct(GfxColorf col)
{
    col.r = sqrtf(col.r);
    col.g = sqrtf(col.g);
    col.b = sqrtf(col.b);
    return col;
}

static inline GfxColorf gfx_color_lerpf(GfxColorf a, GfxColorf b, float t)
{
    return gfx_rgba(GfxColorf,
                    a.r + (b.r - a.r) * t,
                    a.g + (b.g - a.g) * t,
                    a.b + (b.b - a.b) * t,
                    a.a + (b.a - a.a) * t);
}

static inline GfxColor gfx_color_lerpi(GfxColor a, GfxColor b, float t)
{
    return gfx_rgba(GfxColor,
                    a.r + (b.r - a.r) * t,
                    a.g + (b.g - a.g) * t,
                    a.b + (b.b - a.b) * t,
                    a.a + (b.a - a.a) * t);
}
