#pragma once

#include <brutal/base/std.h>

typedef struct
{
    uint8_t r, g, b, a;
} GfxColor;

typedef struct
{
    float r, g, b, a;
} GfxColorf;

#define gfx_colorf_cast(COLOR) \
    ((GfxColorf){(COLOR).r / 255.0f, (COLOR).g / 255.0f, (COLOR).b / 255.0f, (COLOR).a / 255.0f})

#define gfx_colori_cast(COLORF) \
    ((GfxColor){(COLORF).r * 255, (COLORF).g * 255, (COLORF).b * 255, (COLORF).a * 255})

#define GFX_COLOR_BLACK \
    ((GfxColor){0x0, 0x0, 0x0, 0xff})

#define GFX_COLOR_RED \
    ((GfxColor){0xff, 0x0, 0x0, 0xff})

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

        return (GfxColor){r, g, b, 0xff};
    }

    GfxColorf fgf = gfx_colorf_cast(fg);
    GfxColorf bgf = gfx_colorf_cast(fg);

    float a = (1 - fgf.a) * bgf.a + fgf.a;

    float r = ((1 - fgf.a) * bgf.a * bgf.r + fgf.a * fgf.r) / a;
    float g = ((1 - fgf.a) * bgf.a * bgf.g + fgf.a * fgf.g) / a;
    float b = ((1 - fgf.a) * bgf.a * bgf.b + fgf.a * fgf.b) / a;

    GfxColorf res = (GfxColorf){r, g, b, a};
    return gfx_colori_cast(res);
}
