#include <brutal/font/font.h>
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
    vec_init(&self->edges, alloc);
}

void gfx_deinit(Gfx *self)
{
    vec_deinit(&self->edges);
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
        .origin = {},
        .fill = gfx_paint_fill(GFX_WHITE),
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
    gfx_buf_clear(self->buf, color);
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
    gfx_peek(self)->clip = rect;
}

void gfx_origin(Gfx *self, MVec2 pos)
{
    gfx_peek(self)->origin = m_vec2_add(pos, gfx_peek(self)->origin);
}

void gfx_fill(Gfx *self, GfxPaint paint)
{
    gfx_peek(self)->fill = paint;
}

void gfx_no_fill(Gfx *self)
{
    gfx_fill(self, gfx_paint_fill(GFX_BLACK));
}

void gfx_color(Gfx *self, GfxColor color)
{
    gfx_peek(self)->color = color;
}

/* --- Path Building -------------------------------------------------------- */

static void append_edge(void *ctx, MEdge edge)
{
    Gfx *gfx = ctx;
    vec_push(&gfx->edges, edge);
}

void gfx_begin_path(Gfx *self)
{
    vec_clear(&self->edges);

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

static int float_cmp(void const *lhs, void const *rhs)
{
    float const *lhsf = (float const *)lhs;
    float const *rhsf = (float const *)rhs;
    return *lhsf - *rhsf;
}

void gfx_fill_path(Gfx *self, GfxFillRule rule)
{
    MRect pbound = m_edges_bound(vec_begin(&self->edges), vec_len(&self->edges));
    MRect rbound = gfx_buf_bound(self->buf);
    rbound = m_rect_clip_rect(rbound, pbound);
    rbound = m_rect_clip_rect(rbound, gfx_peek(self)->clip);

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

            vec_foreach_v(edge, &self->edges)
            {
                if (m_edge_min_y(edge) <= yy && m_edge_max_y(edge) > yy)
                {
                    float intersection = edge.sx + (yy - edge.sy) / (edge.ey - edge.sy) * (edge.ex - edge.sx);
                    vec_push(&self->active, intersection);
                }
            }

            qsort(self->active.data, self->active.len, sizeof(float), float_cmp);

            bool odd_even = true;
            for (int i = 0; i + 1 < self->active.len; i++)
            {
                float start = m_max(self->active.data[i], m_rect_start(rbound));
                float end = m_min(self->active.data[i + 1], m_rect_end(rbound));

                if (odd_even || rule == GFX_FILL_NONZERO)
                {
                    for (float x = start; x < end; x += 1.0f / RAST_AA)
                    {
                        self->scanline[(int)x] += 1.0;
                    }
                }

                odd_even = !odd_even;
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

void gfx_eval_cmd(Gfx *self, GfxPathCmd cmd)
{
    cmd.cp1 = m_vec2_add(cmd.cp1, gfx_peek(self)->origin);
    cmd.cp2 = m_vec2_add(cmd.cp2, gfx_peek(self)->origin);
    cmd.point = m_vec2_add(cmd.point, gfx_peek(self)->origin);

    gfx_flattener_flatten(&self->flattener, cmd);
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

/* --- Drawing -------------------------------------------------------------- */

void gfx_dot(Gfx *self, MVec2 dot, float size)
{
    gfx_begin_path(self);

    gfx_move_to(self, m_vec2(dot.x - size, dot.x - size));
    gfx_move_to(self, m_vec2(dot.y - size, dot.y + size));
    gfx_move_to(self, m_vec2(dot.x + size, dot.x + size));
    gfx_move_to(self, m_vec2(dot.y + size, dot.y - size));

    gfx_close_path(self);

    gfx_fill_path(self, GFX_FILL_EVENODD);
}

void gfx_line(Gfx *self, MEdge line, float weight)
{
    if (weight < 0)
    {
        weight = -weight;
    }

    float x = line.sx;
    float y = line.sy;
    float dx = line.ex - x;
    float dy = line.ey - y;
    float len = sqrtf(dx * dx + dy * dy);
    float nx = dx / len;
    float ny = dy / len;
    float w = weight / 2;

    gfx_begin_path(self);

    gfx_move_to(self, m_vec2(x - ny * w, y + nx * w));
    gfx_move_to(self, m_vec2(x + ny * w, y - nx * w));
    gfx_move_to(self, m_vec2(x + ny * w, y - nx * w));
    gfx_move_to(self, m_vec2(x - ny * w, y + nx * w));

    gfx_close_path(self);

    gfx_fill_path(self, GFX_FILL_EVENODD);
}

void gfx_rect(Gfx *self, MRect rect, float radius)
{
    gfx_begin_path(self);

    if (radius == 0)
    {
        gfx_move_to(self, m_vec2(rect.x, rect.y));
        gfx_line_to(self, m_vec2(rect.x + rect.width, rect.y));
        gfx_line_to(self, m_vec2(rect.x + rect.width, rect.y + rect.height));
        gfx_line_to(self, m_vec2(rect.x, rect.y + rect.height));
        gfx_close_path(self);
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

    gfx_fill_path(self, GFX_FILL_EVENODD);
}

void gfx_ellipsis(Gfx *self, MRect rect)
{
    gfx_begin_path(self);

    for (size_t i = 0; i < 64; i++)
    {
        float angle = (2 * M_PI * i) / 64;

        gfx_line_to(
            self,
            m_vec2(
                rect.x + rect.width * cosf(angle),
                rect.y + rect.height * sinf(angle)));
    }

    gfx_close_path(self);

    gfx_fill_path(self, GFX_FILL_EVENODD);
}

void gfx_text(Gfx *self, MVec2 origin, Str text, float scale)
{
    origin = m_vec2_add(origin, gfx_peek(self)->origin);
    bfont_render_str(bfont_builtin(), text, origin, self->buf, gfx_peek(self)->clip, scale, gfx_peek(self)->color);
}

void eval_cmd(void *ctx, GfxPathCmd cmd)
{
    Gfx *gfx = ctx;
    gfx_eval_cmd(gfx, cmd);
}

void gfx_path(Gfx *self, Str path)
{
    GfxPathParser parser = {
        .ctx = self,
        .eval = eval_cmd,
    };

    Scan scan;
    scan_init(&scan, path);
    gfx_begin_path(self);
    gfx_path_parse(&parser, &scan);
    gfx_fill_path(self, GFX_FILL_EVENODD);
}
