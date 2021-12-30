#include <brutal/gfx/gfx.h>
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

/* --- Drawing -------------------------------------------------------------- */

void gfx_clear(Gfx *self, GfxColor color)
{
    gfx_buf_clear(self->buf, color);
}

void gfx_dot(Gfx *self, MVec2 dot, float size)
{
    MVec2 vert[4] = {
        m_vec2(dot.x - size, dot.x - size),
        m_vec2(dot.y - size, dot.y + size),
        m_vec2(dot.x + size, dot.x + size),
        m_vec2(dot.y + size, dot.y - size),
    };

    gfx_poly(self, vert, 4);
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

    MVec2 vert[4] = {
        m_vec2(x - ny * w, y + nx * w),
        m_vec2(x + ny * w, y - nx * w),
        m_vec2(x + ny * w, y - nx * w),
        m_vec2(x - ny * w, y + nx * w),
    };

    gfx_poly(self, vert, 4);
}

void gfx_rect(Gfx *self, MRect rect)
{
    MVec2 edges[4] = {
        m_vec2(rect.x, rect.y),
        m_vec2(rect.x + rect.width, rect.y),
        m_vec2(rect.x + rect.width, rect.y + rect.height),
        m_vec2(rect.x, rect.y + rect.height),
    };

    gfx_poly(self, edges, 4);
}

void gfx_ellipsis(Gfx *self, MRect rect)
{
    MVec2 edges[64];

    for (size_t i = 0; i < 64; i++)
    {
        float angle = (2 * M_PI * i) / 64;

        edges[i] = m_vec2(
            rect.x + rect.width * cosf(angle),
            rect.y + rect.height * sinf(angle));
    }

    gfx_poly(self, edges, 64);
}

void gfx_rast(Gfx *self, MVec2 const *edges, size_t len, GfxFillRule rule);

void gfx_poly(Gfx *self, MVec2 const *points, size_t len)
{
    vec_clear(&self->edges);

    for (size_t i = 0; i < len; i++)
    {
        MVec2 p = m_vec2_add(points[i], gfx_peek(self)->origin);
        vec_push(&self->edges, p);
    }

    MVec2 s = m_vec2_add(points[0], gfx_peek(self)->origin);
    vec_push(&self->edges, s);

    gfx_rast(self, self->edges.data, self->edges.len, GFX_RAST_EVENODD);
}

#define RAST_AA 4

static int float_cmp(void const *lhs, void const *rhs)
{
    float const *lhsf = (float const *)lhs;
    float const *rhsf = (float const *)rhs;
    return *lhsf - *rhsf;
}

static inline MRect path_bound(MVec2 const *edges, size_t len)
{
    MRect rect = m_rect_from_points(edges[0], edges[1]);

    for (size_t i = 1; i + 1 < len; i++)
    {
        rect = m_rect_merge_rect(rect, m_rect_from_points(edges[i], edges[i + 1]));
    }

    return rect;
}

FLATTEN void gfx_rast(Gfx *self, MVec2 const *edges, size_t len, GfxFillRule rule)
{
    MRect pbound = path_bound(edges, len);
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

            for (size_t i = 0; i + 1 < len; i++)
            {
                MEdge edge = m_edge(edges[i].x, edges[i].y, edges[i + 1].x, edges[i + 1].y);

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

                if (odd_even || rule == GFX_RAST_NONZERO)
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
