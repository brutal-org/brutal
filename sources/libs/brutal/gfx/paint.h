#pragma once

#include <brutal/debug.h>
#include <brutal/gfx/surface.h>

typedef enum
{
    GFX_PAINT_NONE,

    GFX_PAINT_FILL,
    GFX_PAINT_GRADIENT,
    GFX_PAINT_IMAGE,
} GfxPaintType;

typedef struct
{
    float loc;
    GfxColor color;
} GfxGradientStop;

#define GFX_GRADIENT_MAX_STOPS (16)

typedef struct
{
    size_t len;
    GfxGradientStop stops[GFX_GRADIENT_MAX_STOPS];
} GfxGradient;

typedef struct
{
    GfxPaintType type;

    union
    {
        GfxColor fill;
        GfxGradient gradient;
        GfxSurface image;
    };
} GfxPaint;

#define gfx_paint_none() \
    ((GfxPaint){.type = GFX_PAINT_NONE})

#define gfx_paint_fill(COLOR) \
    ((GfxPaint){.type = GFX_PAINT_FILL, .fill = (COLOR)})

static inline GfxColor gfx_paint_sample(GfxPaint paint, float x, float y)
{
    UNUSED(y);

    switch (paint.type)
    {
    case GFX_PAINT_NONE:
        return GFX_COLOR_MAGENTA;

    case GFX_PAINT_FILL:
        return paint.fill;

    case GFX_PAINT_GRADIENT:
    {
        GfxGradient gradient = paint.gradient;

        if (gradient.len == 0)
        {
            return GFX_COLOR_MAGENTA;
        }
        else if (gradient.len == 1 || x <= gradient.stops[0].loc)
        {
            return gradient.stops[0].color;
        }
        else if (x >= gradient.stops[gradient.len - 1].loc)
        {
            return gradient.stops[gradient.len - 1].color;
        }
        else
        {
            for (size_t i = 0; i + 1 < gradient.len; i++)
            {
                GfxGradientStop a = gradient.stops[i];
                GfxGradientStop b = gradient.stops[i + 1];

                if (a.loc <= x && x < b.loc)
                {
                    float l = ((x - a.loc) / (b.loc - a.loc));
                    return gfx_color_lerpi(a.color, b.color, l);
                }
            }

            return GFX_COLOR_MAGENTA;
        }
    }

    default:
        panic$("Unkown paint type {}.", paint.type);
    }
}
