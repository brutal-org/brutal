#pragma once

#include <brutal/gfx/cache.h>
#include <brutal/gfx/rast.h>

typedef struct
{
    GfxBuf buf;
    GfxCache cache;

    GfxStroke stroke;
    GfxPaint paint;
} GfxCtx;

void gfx_ctx_init(GfxCtx *self, Alloc *alloc, int width, int height, GfxPixelFormat format);

void gfx_ctx_deinit(GfxCtx *self);

void gfx_ctx_begin(GfxCtx *self);

void gfx_ctx_end(GfxCtx *self);

GfxSurface gfx_ctx_surface(GfxCtx *self);

void gfx_ctx_fill(GfxCtx *self, GfxPaint paint);

void gfx_ctx_no_fill(GfxCtx *self);

void gfx_ctx_stroke(GfxCtx *self, GfxStroke paint);

void gfx_ctx_no_stroke(GfxCtx *self);

void gfx_ctx_clear(GfxCtx *self);

void gfx_ctx_rect(GfxCtx *self, Rectf rect);
