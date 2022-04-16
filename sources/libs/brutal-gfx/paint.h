#pragma once

#include <brutal-math>

#include "buf.h"

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

        GfxGradient *gradient_;

        struct
        {
            GfxBuf image;
            MRectf source;
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

static inline GfxPaint gfx_paint_gradient(GfxGradient *grad)
{
    return (GfxPaint){
        .type = GFX_PAINT_GRADIENT,
        .gradient_ = grad,
    };
}

static inline GfxPaint gfx_paint_image(GfxBuf buf, MRectf source)
{
    return (GfxPaint){
        .type = GFX_PAINT_IMAGE,
        .image_ = {
            buf,
            source,
        },
    };
}

static inline bool gfx_paint_is_constant(GfxPaint paint)
{
    switch (paint.type)
    {
    case GFX_PAINT_NONE:
    case GFX_PAINT_FILL:
        return true;
    default:
        return false;
    }
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
        GfxGradient *gradient = paint.gradient_;

        if (gradient->len == 0)
        {
            return GFX_MAGENTA;
        }
        else if (gradient->len == 1 || x <= gradient->stops[0].loc)
        {
            return gradient->stops[0].color;
        }
        else if (x >= gradient->stops[gradient->len - 1].loc)
        {
            return gradient->stops[gradient->len - 1].color;
        }
        else
        {
            for (size_t i = 0; i + 1 < gradient->len; i++)
            {
                GfxGradientStop a = gradient->stops[i];
                GfxGradientStop b = gradient->stops[i + 1];

                if (a.loc <= x && x < b.loc)
                {
                    float l = ((x - a.loc) / (b.loc - a.loc));
                    return gfx_lerp(a.color, b.color, l);
                }
            }

            return GFX_MAGENTA;
        }
    }

    case GFX_PAINT_IMAGE:
    {
        GfxBuf image = paint.image_.image;
        MRectf source = paint.image_.source;

        if (image.width == 0 || image.height == 0)
        {
            return GFX_MAGENTA;
        }
        else if (source.width == 0 || source.height == 0)
        {
            return GFX_MAGENTA;
        }
        else if (x < 0 || x >= source.width)
        {
            return GFX_MAGENTA;
        }
        else if (y < 0 || y >= source.height)
        {
            return GFX_MAGENTA;
        }
        else
        {
            x *= source.width;
            y *= source.height;
            x += source.x;
            y += source.y;

            return gfx_buf_sample(image, x, y);
        }
    }

    default:
        panic$("Unkown paint type {}.", paint.type);
    }
}

static inline bool paint_is_visible(GfxPaint paint)
{
    switch (paint.type)
    {
    case GFX_PAINT_NONE:
        return false;

    case GFX_PAINT_FILL:
        return paint.fill_.a != 255;

    case GFX_PAINT_GRADIENT:
        return true;

    default:
        panic$("Unkown paint type {}.", paint.type);
    }
}
