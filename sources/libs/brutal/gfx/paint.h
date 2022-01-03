#pragma once

#include <brutal/gfx/buf.h>
#include <brutal/math/edge.h>
#include <brutal/math/trans2.h>

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
        GfxColor fill_;

        GfxGradient gradient_;

        struct
        {
            GfxBuf image;
            MRect source;
        } image_;
    };
} GfxPaint;

static inline GfxPaint gfx_paint_none(void)
{
    return (GfxPaint){
        .type = GFX_PAINT_NONE,
    };
}

static inline GfxPaint gfx_paint_fill(GfxColor color)
{
    return (GfxPaint){
        .type = GFX_PAINT_FILL,
        .fill_ = color,
    };
}

static inline GfxPaint gfx_paint_gradient(GfxGradient grad)
{
    return (GfxPaint){
        .type = GFX_PAINT_GRADIENT,
        .gradient_ = grad,
    };
}

static inline GfxPaint gfx_paint_image(GfxBuf buf, MRect source)
{
    return (GfxPaint){
        .type = GFX_PAINT_IMAGE,
        .image_ = {
            buf,
            source,
        },
    };
}

static inline GfxColor gfx_paint_sample(GfxPaint paint, float x, float y)
{
    UNUSED(y);

    switch (paint.type)
    {
    case GFX_PAINT_NONE:
        return GFX_MAGENTA;

    case GFX_PAINT_FILL:
        return paint.fill_;

    case GFX_PAINT_GRADIENT:
    {
        GfxGradient gradient = paint.gradient_;

        if (gradient.len == 0)
        {
            return GFX_MAGENTA;
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
                    return gfx_lerp(a.color, b.color, l);
                }
            }

            return GFX_MAGENTA;
        }
    }

    default:
        panic$("Unkown paint type {}.", paint.type);
    }
}
