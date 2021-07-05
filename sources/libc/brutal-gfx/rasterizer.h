#pragma once

#include <brutal-gfx/paint.h>
#include <brutal-gfx/path.h>
#include <brutal-gfx/stroke.h>

typedef struct
{

} GfxRasterizer;

void gfx_rasterizer_init(GfxRasterizer *self);

void gfx_rasterizer_deinit(GfxRasterizer *self);

void gfx_rasterizer_fill(
    GfxRasterizer *self, GfxSurface surface, GfxPath path, GfxPaint paint);

void gfx_rasterizer_stroke(
    GfxRasterizer *self, GfxSurface surface, GfxPath path, GfxStroke stroke, GfxPaint paint);
