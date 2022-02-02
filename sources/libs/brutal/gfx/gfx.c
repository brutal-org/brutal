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

static int float_cmp(void const *lhs, void const *rhs)
{
    float const *lhsf = (float const *)lhs;
    float const *rhsf = (float const *)rhs;
    return *lhsf - *rhsf;
}

void gfx_fill_path(Gfx *self, GfxFillRule rule)
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

void gfx_stroke_path(Gfx *self, float width)
{
    vec_clear(&self->stroke);

    for (int i = 0; i < vec_len(&self->path); i++)
    {
        MEdge a = vec_at(&self->path, i);
        MEdge b = vec_at(&self->path, (i + 1) % vec_len(&self->path));

        MEdge aa = m_edge_parallel(a, width / 2);
        MEdge ab = m_edge_parallel(a, -width / 2);

        MEdge ba = m_edge_parallel(b, width / 2);
        MEdge bb = m_edge_parallel(b, -width / 2);

        vec_push(&self->stroke, aa);
        vec_push(&self->stroke, ab);

        vec_push(&self->stroke, m_edge_vec2(aa.end, ba.start));
        vec_push(&self->stroke, m_edge_vec2(ab.end, bb.start));
    }

    swap$(&self->path, &self->stroke);
    gfx_fill_path(self, GFX_FILL_EVENODD);
    swap$(&self->path, &self->stroke);
}

void gfx_eval_cmd(Gfx *self, GfxPathCmd cmd)
{
    cmd.cp1 = m_vec2_add(cmd.cp1, gfx_peek(self)->origin);
    cmd.cp2 = m_vec2_add(cmd.cp2, gfx_peek(self)->origin);
    cmd.point = m_vec2_add(cmd.point, gfx_peek(self)->origin);

    gfx_flattener_flatten(&self->flattener, cmd);
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

/* --- Drawing -------------------------------------------------------------- */

void gfx_dot(Gfx *self, MVec2 dot, float size)
{
    gfx_begin_path(self);

    gfx_move_to(self, m_vec2(dot.x - size, dot.y - size));
    gfx_line_to(self, m_vec2(dot.x + size, dot.y - size));
    gfx_line_to(self, m_vec2(dot.x + size, dot.y + size));
    gfx_line_to(self, m_vec2(dot.x - size, dot.y + size));

    gfx_close_path(self);

    gfx_fill_path(self, GFX_FILL_EVENODD);
}

void gfx_fill_line(Gfx *self, MEdge line, float weight)
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

    gfx_fill_path(self, GFX_FILL_EVENODD);
}

void gfx_fill_rect_aligned(Gfx *self, MRect rect)
{
    if (m_rect_empty(rect))
    {
        return;
    }

    rect.pos = m_vec2_add(rect.pos, gfx_peek(self)->origin);
    rect = m_rect_clip_rect(rect, gfx_peek(self)->clip);

    GfxColor color = gfx_paint_sample(gfx_peek(self)->fill, 0, 0);

    for (int y = rect.y; y < rect.y + rect.height; y++)
    {
        for (int x = rect.x; x < rect.x + rect.width; x++)
        {
            gfx_buf_blend(self->buf, x, y, color);
        }
    }
}

void gfx_fill_rect(Gfx *self, MRect rect, float radius)
{
    gfx_begin_path(self);

    if (radius == 0)
    {
        gfx_fill_rect_aligned(self, rect);
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

    gfx_close_path(self);

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

void gfx_fill_text(Gfx *self, MVec2 origin, Str text, GfxFont font)
{
    origin = m_vec2_add(origin, gfx_peek(self)->origin);
    gfx_font_render_str(font, self, origin, text);
}

void gfx_fill_svg(Gfx *self, Str path)
{
    gfx_begin_path(self);
    gfx_eval_svg(self, path);
    gfx_fill_path(self, GFX_FILL_EVENODD);
}
