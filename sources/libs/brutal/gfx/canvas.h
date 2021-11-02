#pragma once

#include <brutal/gfx/cache.h>
#include <brutal/gfx/rast.h>

typedef struct
{
    GfxBuf buf;
    GfxCache cache;

    GfxStroke stroke;
    GfxPaint paint;
} GfxCanvas;

void gfx_canvas_init(GfxCanvas *self, Alloc *alloc, int width, int height, GfxPixelFormat format);

void gfx_canvas_deinit(GfxCanvas *self);

void gfx_canvas_begin(GfxCanvas *self);

void gfx_canvas_end(GfxCanvas *self);

GfxSurface gfx_canvas_surface(GfxCanvas *self);

void gfx_canvas_fill(GfxCanvas *self, GfxPaint paint);

void gfx_canvas_no_fill(GfxCanvas *self);

void gfx_canvas_stroke(GfxCanvas *self, GfxStroke paint);

void gfx_canvas_no_stroke(GfxCanvas *self);

void gfx_canvas_clear(GfxCanvas *self);

void gfx_canvas_rect(GfxCanvas *self, Rectf rect);
