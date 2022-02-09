#pragma once

#include <brutal/ds.h>
#include <brutal/gfx/font.h>
#include <brutal/gfx/paint.h>
#include <brutal/gfx/path.h>
#include <brutal/gfx/stroke.h>
#include <brutal/hash.h>
#include <brutal/math.h>

typedef struct
{
    MRect clip;
    MVec2 origin;
    GfxPaint fill;
    GfxStroke stroke;
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

void gfx_fill_style(Gfx *self, GfxPaint paint);

void gfx_fill_reset(Gfx *self);

void gfx_stroke_style(Gfx *self, GfxStroke stroke);

void gfx_stroke_reset(Gfx *self);

void gfx_color(Gfx *self, GfxColor color);

/* --- Path Building -------------------------------------------------------- */

void gfx_begin_path(Gfx *self);

void gfx_close_path(Gfx *self);

void gfx_fill(Gfx *self, GfxFillRule rule);

void gfx_stroke(Gfx *self);

void gfx_debug(Gfx *self);

void gfx_eval_cmd(Gfx *self, GfxPathCmd cmd);

void gfx_eval_path(Gfx *self, GfxPath *path);

void gfx_eval_svg(Gfx *self, Str path);

void gfx_move_to(Gfx *self, MVec2 point);

void gfx_line_to(Gfx *self, MVec2 point);

void gfx_bezier_to(Gfx *self, MVec2 cp1, MVec2 cp2, MVec2 point);

void gfx_quadratic_to(Gfx *self, MVec2 cp2, MVec2 point);

void gfx_arc_to(Gfx *self, float rx, float ry, float angle, int flags, MVec2 point);

void gfx_rect(Gfx *self, MRect rect, float radius);

void gfx_ellipse(Gfx *self, MRect rect);

/* --- Drawing -------------------------------------------------------------- */

void gfx_dot(Gfx *self, MVec2 dot, float size);

void gfx_line(Gfx *self, MEdge line, float weight);

void gfx_text(Gfx *self, MVec2 origin, Str text, GfxFont font);

void gfx_fill_rect(Gfx *self, MRect rect, float radius);

void gfx_stroke_rect(Gfx *self, MRect rect, float radius);

void gfx_fill_ellipse(Gfx *self, MRect rect);

void gfx_stroke_ellipse(Gfx *self, MRect rect);

void gfx_fill_path(Gfx *self, GfxPath *path, GfxFillRule rule);

void gfx_stroke_path(Gfx *self, GfxPath *path);

void gfx_fill_svg(Gfx *self, Str path, GfxFillRule rule);

void gfx_stroke_svg(Gfx *self, Str path);
