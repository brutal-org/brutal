#pragma once

#include <brutal/ds.h>
#include <brutal/gfx/rast.h>
#include <brutal/hash.h>
#include <brutal/math.h>

typedef struct
{
    Recti clip;
    Vec2i origine;
    GfxPaint fill;
} GfxCtx;

typedef enum
{
    GFX_RAST_EVENODD,
    GFX_RAST_NONZERO,
} GfxFillRule;

typedef struct
{
    bool begin;
    Alloc *alloc;
    GfxBuf buf;
    Vec(GfxCtx) ctx;
    Vec(Vec2f) edges;
    Vec(float) active;

    float *scanline;
    int scanline_len;
} Gfx;

void gfx_init(Gfx *self, Alloc *alloc);

void gfx_deinit(Gfx *self);

void gfx_begin(Gfx *self, GfxBuf buf);

void gfx_end(Gfx *self);

void gfx_push(Gfx *self);

void gfx_pop(Gfx *self);

void gfx_clip(Gfx *self, Recti rect);

void gfx_origine(Gfx *self, Vec2i pos);

void gfx_fill(Gfx *self, GfxPaint paint);

void gfx_no_fill(Gfx *self);

void gfx_clear(Gfx *self, GfxColor color);

void gfx_dot(Gfx *self, Vec2f dot, float size);

void gfx_line(Gfx *self, Edgef line, float weight);

void gfx_rect(Gfx *self, Rectf rect);

void gfx_ellipse(Gfx *self, Rectf rect);

void gfx_poly(Gfx *self, Vec2f const *points, size_t len);
