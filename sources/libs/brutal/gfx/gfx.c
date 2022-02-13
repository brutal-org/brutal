#include <brutal/base/macros.h>
#include <brutal/gfx/gfx.h>
#include <brutal/gfx/path.h>
#include <stdlib.h>

/* --- Lifetime ------------------------------------------------------------- */

void gfx_init(Gfx *self, Alloc *alloc)
{
    *self = (Gfx){};

    self->alloc = alloc;
    vec_init(&self->ctx, alloc);
    vec_init(&self->active, alloc);
    vec_init(&self->path, alloc);
    vec_init(&self->stroke, alloc);
}

void gfx_deinit(Gfx *self)
{
    vec_deinit(&self->path);
    vec_deinit(&self->stroke);
    vec_deinit(&self->active);
    vec_deinit(&self->ctx);

    if (self->scanline)
    {
        alloc_free(self->alloc, self->scanline);
    }
}

/* --- Cycle ---------------------------------------------------------------- */

void gfx_begin(Gfx *self, GfxBuf buf)
{
    assert_truth(!self->begin);
    self->begin = true;
    self->buf = buf;

    if (!self->scanline || self->scanline_len < buf.width)
    {
        if (self->scanline)
        {
            alloc_free(self->alloc, self->scanline);
        }

        self->scanline_len = buf.width;
        self->scanline = alloc_malloc(self->alloc, self->scanline_len * sizeof(*self->scanline));
    }

    GfxCtx ctx = (GfxCtx){
        .clip = gfx_buf_bound(buf),
        .trans = m_trans2_identity(),
        .fill = gfx_paint_fill(GFX_WHITE),
        .stroke = (GfxStroke){
            .width = 1,
        },
        .font_family = gfx_font_builtin(),
        .font_style = GFX_FONT_DEFAULT,
        .color = GFX_WHITE,
    };

    vec_push(&self->ctx, ctx);
}

void gfx_end(Gfx *self)
{
    assert_truth(self->begin);
    self->begin = false;

    assert_equal(self->ctx.len, 1);
    (void)vec_pop(&self->ctx);
}

void gfx_clear(Gfx *self, GfxColor color)
{
    gfx_buf_clear_rect(self->buf, gfx_peek(self)->clip, color);
}

/* --- Context -------------------------------------------------------------- */

void gfx_push(Gfx *self)
{
    GfxCtx ctx = vec_last(&self->ctx);
    vec_push(&self->ctx, ctx);
}

void gfx_pop(Gfx *self)
{
    (void)vec_pop(&self->ctx);
}

GfxCtx *gfx_peek(Gfx *self)
{
    return &vec_last(&self->ctx);
}

void gfx_clip(Gfx *self, MRect rect)
{
    rect.pos = m_vec2_add(rect.pos, gfx_peek(self)->origin);
    rect = m_rect_clip_rect(rect, gfx_peek(self)->clip);
    gfx_peek(self)->clip = rect;
}

void gfx_origin(Gfx *self, MVec2 pos)
{
    gfx_peek(self)->origin = m_vec2_add(pos, gfx_peek(self)->origin);
}

void gfx_trans(Gfx *self, MTrans2 trans)
{
    gfx_peek(self)->trans = trans;
}

void gfx_stroke_style(Gfx *self, GfxStroke stroke)
{
    gfx_peek(self)->stroke = stroke;
}

void gfx_stroke_reset(Gfx *self)
{
    gfx_stroke_style(self, (GfxStroke){.width = 1});
}

void gfx_fill_style(Gfx *self, GfxPaint paint)
{
    gfx_peek(self)->fill = paint;
}

void gfx_fill_reset(Gfx *self)
{
    gfx_fill_style(self, gfx_paint_fill(GFX_BLACK));
}

void gfx_font_family(Gfx *self, GfxFont family)
{
    gfx_peek(self)->font_family = family;
}

void gfx_font_style(Gfx *self, GfxFontStyle style)
{
    gfx_peek(self)->font_style = style;
}

void gfx_color(Gfx *self, GfxColor color)
{
    gfx_peek(self)->color = color;
}

/* --- Path Building -------------------------------------------------------- */

static void append_edge(void *ctx, MEdge edge)
{
    Gfx *gfx = ctx;
    vec_push(&gfx->path, edge);
}

void gfx_begin_path(Gfx *self)
{
    vec_clear(&self->path);

    self->flattener.ctx = self;
    self->flattener.append = append_edge;

    gfx_flattener_begin(&self->flattener);
}

void gfx_close_path(Gfx *self)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_CLOSE_PATH,
    };

    gfx_eval_cmd(self, cmd);
}

#define RAST_AA 4

static int gfx_active_edge_cmp(void const *lhs, void const *rhs)
{
    GfxActiveEdge const *lhsf = lhs;
    GfxActiveEdge const *rhsf = rhs;

    return lhsf->x - rhsf->x;
}

void gfx_fill(Gfx *self, GfxFillRule rule)
{
    MRect pbound = m_edges_bound(vec_begin(&self->path), vec_len(&self->path));
    MRect rbound = m_rect_clip_rect(pbound, gfx_peek(self)->clip);

    if (m_rect_empty(rbound))
    {
        return;
    }

    for (int y = m_rect_top(rbound); y < m_rect_bottom(rbound); y++)
    {
        mem_set(self->scanline, 0, sizeof(*self->scanline) * self->scanline_len);

        for (float yy = (y); yy < y + 1; yy += 1.0f / RAST_AA)
        {
            vec_clear(&self->active);

            vec_foreach_v(edge, &self->path)
            {
                if (m_edge_min_y(edge) <= yy && m_edge_max_y(edge) > yy)
                {
                    GfxActiveEdge active;
                    active.x = edge.sx + (yy - edge.sy) / (edge.ey - edge.sy) * (edge.ex - edge.sx);
                    active.winding = edge.sy > edge.ey ? 1 : -1;
                    vec_push(&self->active, active);
                }
            }

            if (vec_len(&self->active) > 0)
            {
                qsort(self->active.data, self->active.len, sizeof(GfxActiveEdge), gfx_active_edge_cmp);

                int r = 0;

                for (int i = 0; i + 1 < self->active.len; i++)
                {
                    GfxActiveEdge start_edge = vec_at(&self->active, i);
                    GfxActiveEdge end_edge = vec_at(&self->active, i + 1);

                    r += start_edge.winding;

                    if ((rule == GFX_FILL_EVENODD && r % 2) || (rule == GFX_FILL_NONZERO && r != 0))
                    {
                        for (float x = start_edge.x; x < end_edge.x; x += 1.0f / RAST_AA)
                        {
                            self->scanline[(int)x] += 1.0;
                        }
                    }
                }
            }
        }

        for (int x = m_rect_start(rbound); x < m_rect_end(rbound); x++)
        {
            float alpha = m_clamp(self->scanline[x] / (RAST_AA * RAST_AA), 0, 1);

            if (alpha >= 0.003f)
            {
                float sx = (x - pbound.x) / pbound.width;
                float sy = (y - pbound.y) / pbound.height;

                GfxColor color = gfx_paint_sample(gfx_peek(self)->fill, sx, sy);
                color.a = color.a * alpha;

                gfx_buf_blend(self->buf, x, y, color);
            }
        }
    }
}

void gfx_stroke(Gfx *self)
{
    vec_clear(&self->stroke);
    gfx_stroke_apply(gfx_peek(self)->stroke, &self->path, &self->stroke);
    swap$(&self->path, &self->stroke);
    gfx_fill(self, GFX_FILL_NONZERO);
    swap$(&self->path, &self->stroke);
}

void gfx_debug(Gfx *self)
{
    vec_foreach_v(edge, &self->path)
    {
        gfx_line(self, edge, 1);
    }
}

void gfx_eval_cmd(Gfx *self, GfxPathCmd cmd)
{
    MTrans2 trans = gfx_peek(self)->trans;

    if (!m_trans2_is_identity(trans))
    {
        cmd.cp1 = m_trans2_apply_point(trans, cmd.cp1);
        cmd.cp2 = m_trans2_apply_point(trans, cmd.cp2);
        cmd.point = m_trans2_apply_point(trans, cmd.point);
    }

    MVec2 origin = gfx_peek(self)->origin;

    cmd.cp1 = m_vec2_add(origin, cmd.cp1);
    cmd.cp2 = m_vec2_add(origin, cmd.cp2);
    cmd.point = m_vec2_add(origin, cmd.point);

    gfx_flattener_flatten(&self->flattener, cmd);
}

void gfx_eval_path(Gfx *self, GfxPath *path)
{
    vec_foreach_v(cmd, path)
    {
        gfx_eval_cmd(self, cmd);
    }
}

static void eval_cmd(void *ctx, GfxPathCmd cmd)
{
    Gfx *gfx = ctx;
    gfx_eval_cmd(gfx, cmd);
}

void gfx_eval_svg(Gfx *self, Str path)
{
    GfxPathParser parser = {
        .ctx = self,
        .eval = eval_cmd,
    };

    Scan scan;
    scan_init(&scan, path);
    gfx_path_parse(&parser, &scan);
}

void gfx_move_to(Gfx *self, MVec2 point)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_MOVE_TO,
        .point = point,
    };

    gfx_eval_cmd(self, cmd);
}

void gfx_line_to(Gfx *self, MVec2 point)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_LINE_TO,
        .point = point,
    };

    gfx_eval_cmd(self, cmd);
}

void gfx_bezier_to(Gfx *self, MVec2 cp1, MVec2 cp2, MVec2 point)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_CUBIC_TO,
        .cp1 = cp1,
        .cp2 = cp2,
        .point = point,
    };

    gfx_eval_cmd(self, cmd);
}

void gfx_quadratic_to(Gfx *self, MVec2 cp, MVec2 point)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_QUADRATIC_TO,
        .cp = cp,
        .point = point,
    };

    gfx_eval_cmd(self, cmd);
}

void gfx_arc_to(Gfx *self, float rx, float ry, float angle, int flags, MVec2 point)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_ARC_TO,
        .rx = rx,
        .ry = ry,
        .angle = angle,
        .flags = flags,
        .point = point,
    };

    gfx_eval_cmd(self, cmd);
}

void gfx_rect(Gfx *self, MRect rect, float radius)
{
    if (radius <= 0.5)
    {
        gfx_move_to(self, m_vec2(rect.x + radius, rect.y));

        gfx_line_to(self, m_vec2(rect.x + rect.width, rect.y));
        gfx_line_to(self, m_vec2(rect.x + rect.width, rect.y + rect.height));
        gfx_line_to(self, m_vec2(rect.x, rect.y + rect.height));
        gfx_line_to(self, m_vec2(rect.x + radius, rect.y));
    }
    else
    {
        gfx_move_to(self, m_vec2(rect.x + radius, rect.y));

        // Top edge
        gfx_line_to(self, m_vec2(rect.x + rect.width - radius, rect.y));
        gfx_quadratic_to(self, m_vec2(rect.x + rect.width, rect.y), m_vec2(rect.x + rect.width, rect.y + radius));

        // Right edge
        gfx_line_to(self, m_vec2(rect.x + rect.width, rect.y + rect.height - radius));
        gfx_quadratic_to(self, m_vec2(rect.x + rect.width, rect.y + rect.height), m_vec2(rect.x + rect.width - radius, rect.y + rect.height));

        // Bottom edge
        gfx_line_to(self, m_vec2(rect.x + radius, rect.y + rect.height));
        gfx_quadratic_to(self, m_vec2(rect.x, rect.y + rect.height), m_vec2(rect.x, rect.y + rect.height - radius));

        // Left edge
        gfx_line_to(self, m_vec2(rect.x, rect.y + radius));
        gfx_quadratic_to(self, m_vec2(rect.x, rect.y), m_vec2(rect.x + radius, rect.y));
    }
}

void gfx_ellipse(Gfx *self, MRect rect)
{
    for (size_t i = 0; i < 64; i++)
    {
        float angle = (2 * M_PI * i) / 64;

        gfx_line_to(
            self,
            m_vec2(
                rect.x + rect.width * cosf(angle),
                rect.y + rect.height * sinf(angle)));
    }
}

/* --- Drawing -------------------------------------------------------------- */

void gfx_dot(Gfx *self, MVec2 dot, float size)
{
    gfx_begin_path(self);

    gfx_move_to(self, m_vec2(dot.x - size, dot.y - size));
    gfx_line_to(self, m_vec2(dot.x + size, dot.y - size));
    gfx_line_to(self, m_vec2(dot.x + size, dot.y + size));
    gfx_line_to(self, m_vec2(dot.x - size, dot.y + size));

    gfx_close_path(self);

    gfx_fill(self, GFX_FILL_EVENODD);
}

void gfx_line(Gfx *self, MEdge line, float weight)
{
    float sx = line.sx;
    float sy = line.sy;
    float ex = line.ex;
    float ey = line.ey;

    float dx = line.ex - sx;
    float dy = line.ey - sy;
    float len = sqrtf(dx * dx + dy * dy);
    float nx = dx / len;
    float ny = dy / len;
    float w = weight / 2;

    gfx_begin_path(self);

    gfx_move_to(self, m_vec2(sx - ny * w, sy + nx * w));
    gfx_line_to(self, m_vec2(sx + ny * w, sy - nx * w));

    gfx_line_to(self, m_vec2(ex + ny * w, ey - nx * w));
    gfx_line_to(self, m_vec2(ex - ny * w, ey + nx * w));

    gfx_close_path(self);

    gfx_fill(self, GFX_FILL_EVENODD);
}

void gfx_text(Gfx *self, MVec2 origin, Str text)
{
    GfxFont family = gfx_peek(self)->font_family;
    GfxFontStyle style = gfx_peek(self)->font_style;
    gfx_font_render_str(family, style, self, origin, text);
}

void gfx_fill_rect_aligned(Gfx *self, MRect rect)
{
    if (m_rect_empty(rect))
    {
        return;
    }

    rect.pos = m_vec2_add(rect.pos, gfx_peek(self)->origin);
    MRect rbound = m_rect_clip_rect(rect, gfx_peek(self)->clip);

    for (int y = m_rect_top(rbound); y < m_rect_bottom(rbound); y++)
    {
        for (int x = m_rect_start(rbound); x < m_rect_end(rbound); x++)
        {
            float sx = (x - rect.x) / rect.width;
            float sy = (y - rect.y) / rect.height;

            GfxColor color = gfx_paint_sample(gfx_peek(self)->fill, sx, sy);
            gfx_buf_blend(self->buf, x, y, color);
        }
    }
}

void gfx_fill_rect(Gfx *self, MRect rect, float radius)
{
    gfx_begin_path(self);
    gfx_rect(self, rect, radius);
    gfx_close_path(self);

    if (radius == 0 && m_trans2_is_identity(gfx_peek(self)->trans))
    {
        gfx_fill_rect_aligned(self, rect);
    }
    else
    {
        gfx_fill(self, GFX_FILL_EVENODD);
    }
}

void gfx_stroke_rect(Gfx *self, MRect rect, float radius)
{
    gfx_begin_path(self);
    gfx_rect(self, rect, radius);
    gfx_close_path(self);

    gfx_stroke(self);
}

void gfx_fill_ellipse(Gfx *self, MRect rect)
{
    gfx_begin_path(self);
    gfx_ellipse(self, rect);
    gfx_close_path(self);

    gfx_fill(self, GFX_FILL_EVENODD);
}

void gfx_stroke_ellipse(Gfx *self, MRect rect)
{
    gfx_begin_path(self);
    gfx_ellipse(self, rect);
    gfx_close_path(self);

    gfx_stroke(self);
}

void gfx_fill_path(Gfx *self, GfxPath *path, GfxFillRule rule)
{
    gfx_begin_path(self);
    gfx_eval_path(self, path);
    gfx_fill(self, rule);
}

void gfx_stroke_path(Gfx *self, GfxPath *path)
{
    gfx_begin_path(self);
    gfx_eval_path(self, path);
    gfx_stroke(self);
}

void gfx_fill_svg(Gfx *self, Str path, GfxFillRule rule)
{
    gfx_begin_path(self);
    gfx_eval_svg(self, path);
    gfx_fill(self, rule);
}

void gfx_stroke_svg(Gfx *self, Str path)
{
    gfx_begin_path(self);
    gfx_eval_svg(self, path);
    gfx_stroke(self);
}
