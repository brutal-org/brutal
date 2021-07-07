#pragma once

#include <brutal/gfx/edge.h>
#include <brutal/gfx/paint.h>
#include <brutal/gfx/stroke.h>
#include <brutal/gfx/transform.h>

typedef struct
{
    GfxEdgeListBuffer stroke;
} GfxRast;

void gfx_rast_init(GfxRast *self);

void gfx_rast_deinit(GfxRast *self);

void gfx_rast_fill(
    GfxRast *self, GfxSurface surface, GfxEdgeList edges, GfxPaint paint);

void gfx_rast_stroke(
    GfxRast *self, GfxSurface surface, GfxEdgeList edges, GfxStroke stroke, GfxPaint paint);
