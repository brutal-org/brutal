#pragma once

#include <brutal/ds.h>
#include <brutal/gfx/font.h>
#include <brutal/gfx/paint.h>
#include <brutal/gfx/path.h>
#include <brutal/hash.h>
#include <brutal/math.h>

typedef struct
{
    MRect clip;
    MVec2 origin;
    GfxPaint fill;
    GfxColor color;
} GfxCtx;

typedef enum
{
    GFX_FILL_EVENODD,
    GFX_FILL_NONZERO,
} GfxFillRule;

typedef struct
{
    float x;
    int winding;
} GfxActiveEdge;

typedef struct _Gfx
{
    bool begin;
    Alloc *alloc;
    GfxBuf buf;
    Vec(GfxCtx) ctx;

    MEdges path;
    MEdges stroke;

    Vec(GfxActiveEdge) active;
    GfxPathFlattener flattener;

    float *scanline;
    int scanline_len;
} Gfx;

/* --- Lifetime ------------------------------------------------------------- */

void gfx_init(Gfx *self, Alloc *alloc);

void gfx_deinit(Gfx *self);

/* --- Cycle ---------------------------------------------------------------- */

void gfx_begin(Gfx *self, GfxBuf buf);

void gfx_end(Gfx *self);

void gfx_clear(Gfx *self, GfxColor color);

/* --- Context -------------------------------------------------------------- */

void gfx_push(Gfx *self);

void gfx_pop(Gfx *self);

GfxCtx *gfx_peek(Gfx *self);

void gfx_clip(Gfx *self, MRect rect);

void gfx_origin(Gfx *self, MVec2 pos);

void gfx_fill(Gfx *self, GfxPaint paint);

void gfx_no_fill(Gfx *self);

void gfx_color(Gfx *self, GfxColor color);

/* --- Path Building -------------------------------------------------------- */

void gfx_begin_path(Gfx *self);

void gfx_close_path(Gfx *self);

void gfx_fill_path(Gfx *self, GfxFillRule rule);

void gfx_stroke_path(Gfx *self, float width);

void gfx_eval_cmd(Gfx *self, GfxPathCmd cmd);

void gfx_eval_svg(Gfx *self, Str path);

void gfx_move_to(Gfx *self, MVec2 point);

void gfx_line_to(Gfx *self, MVec2 point);

void gfx_bezier_to(Gfx *self, MVec2 cp1, MVec2 cp2, MVec2 point);

void gfx_quadratic_to(Gfx *self, MVec2 cp2, MVec2 point);

void gfx_arc_to(Gfx *self, float rx, float ry, float angle, int flags, MVec2 point);

void gfx_rect(Gfx *self, MRect rect, float radius);

/* --- Drawing -------------------------------------------------------------- */

void gfx_dot(Gfx *self, MVec2 dot, float size);

void gfx_fill_line(Gfx *self, MEdge line, float weight);

void gfx_fill_rect(Gfx *self, MRect rect, float radius);

void gfx_fill_ellipse(Gfx *self, MRect rect);

void gfx_fill_text(Gfx *self, MVec2 origin, Str text, GfxFont font);

void gfx_fill_svg(Gfx *self, Str path);
