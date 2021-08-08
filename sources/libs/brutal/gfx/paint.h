#pragma once

#include <brutal/gfx/surface.h>
#include <brutal/log.h>

typedef enum
{
    GFX_PAINT_NONE,

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

#define gfx_paint_none() \
    ((GfxPaint){.type = GFX_PAINT_NONE})

#define gfx_paint_fill(COLOR) \
    ((GfxPaint){.type = GFX_PAINT_FILL, .fill = (COLOR)})

static inline GfxColor gfx_paint_sample(GfxPaint *paint, MAYBE_UNUSED float x, MAYBE_UNUSED float y)
{
    switch (paint->type)
    {
    case GFX_PAINT_FILL:
        return paint->fill;

    default:
        panic_todo("Implement other paint types.");
    }
}
