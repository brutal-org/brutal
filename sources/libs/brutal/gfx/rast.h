#pragma once

#include <brutal/gfx/paint.h>
#include <brutal/gfx/stroke.h>
#include <brutal/gfx/surface.h>
#include <brutal/math/edge.h>
#include <brutal/math/trans2.h>

typedef enum
{
    GFX_RAST_EVENODD,
    GFX_RAST_NONZERO,
} GfxRastRule;

void gfx_rast_fill(
    GfxSurface surface,
    Edgesf edges,
    Recti clip,
    GfxPaint paint,
    GfxRastRule rule,
    Alloc *alloc);
