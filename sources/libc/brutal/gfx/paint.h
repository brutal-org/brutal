#pragma once

#include <brutal/gfx/surface.h>
#include <brutal/log.h>

typedef enum
{
    GFX_PAINT_FILL,
    GFX_PAINT_GRADIANT,
    GFX_PAINT_IMAGE,
} GfxPaintType;

typedef struct
{
    float location;
    GfxColor color;
} GfxGradiantStop;

typedef struct
{
    size_t len;
    GfxGradiantStop stops[16];
} GfxGradiant;

typedef struct
{
    GfxPaintType type;

    union
    {
        GfxColor fill;
        GfxGradiant gradiant;
        GfxSurface image;
    };
} GfxPaint;

static inline GfxColor gfx_paint_sample(GfxPaint *paint, MAYBE_UNUSED float x, MAYBE_UNUSED float y)
{
    switch (paint->type)
    {
    case GFX_PAINT_FILL:
        return paint->fill;

    default:
        todo("Implement other paint types.");
    }
}
