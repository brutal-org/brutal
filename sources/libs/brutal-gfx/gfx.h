#pragma once

#include <brutal-ds>
#include <brutal-math>

#include "font.h"
#include "paint.h"
#include "path.h"
#include "stroke.h"

typedef struct
{
    MRectf clip;
    MVec2f origin;
    MTrans2 trans;

    GfxPaint fill;
    GfxStroke stroke;

    GfxFont font_family;
    GfxFontStyle font_style;
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

void gfx_clip(Gfx *self, MRectf rect);

void gfx_origin(Gfx *self, MVec2f pos);

void gfx_trans(Gfx *self, MTrans2 trans);

void gfx_fill_style(Gfx *self, GfxPaint paint);

void gfx_fill_reset(Gfx *self);

void gfx_stroke_style(Gfx *self, GfxStroke stroke);

void gfx_stroke_reset(Gfx *self);

void gfx_font_family(Gfx *self, GfxFont font);

void gfx_font_style(Gfx *self, GfxFontStyle style);

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

void gfx_move_to(Gfx *self, MVec2f point);

void gfx_line_to(Gfx *self, MVec2f point);

void gfx_bezier_to(Gfx *self, MVec2f cp1, MVec2f cp2, MVec2f point);

void gfx_quadratic_to(Gfx *self, MVec2f cp2, MVec2f point);

void gfx_arc_to(Gfx *self, float rx, float ry, float angle, int flags, MVec2f point);

void gfx_rect(Gfx *self, MRectf rect, float radius);

void gfx_ellipse(Gfx *self, MRectf rect);

/* --- Drawing -------------------------------------------------------------- */

void gfx_dot(Gfx *self, MVec2f dot, float size);

void gfx_line(Gfx *self, MEdge line, float weight);

void gfx_text(Gfx *self, MVec2f origin, Str text);

void gfx_fill_rect(Gfx *self, MRectf rect, float radius);

void gfx_stroke_rect(Gfx *self, MRectf rect, float radius);

void gfx_fill_ellipse(Gfx *self, MRectf rect);

void gfx_stroke_ellipse(Gfx *self, MRectf rect);

void gfx_fill_path(Gfx *self, GfxPath *path, GfxFillRule rule);

void gfx_stroke_path(Gfx *self, GfxPath *path);

void gfx_fill_svg(Gfx *self, Str path, GfxFillRule rule);

void gfx_stroke_svg(Gfx *self, Str path);
