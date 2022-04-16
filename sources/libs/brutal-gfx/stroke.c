#include <brutal-debug>

#include "stroke.h"

void gfx_stroke_join(GfxStrokeJoin join, MEdges *stroke, MEdge curr, MEdge next, MAYBE_UNUSED float dist)
{
    if (join == GFX_STROKE_JOIN_MITTER)
    {
        MVec2f curr_vec = m_vec2f_sub(curr.end, curr.start);
        MVec2f next_vec = m_vec2f_sub(next.start, next.end);
        MVec2f diff_vec = m_vec2f_sub(next.start, curr.start);

        float j = m_vec2f_dot(next_vec, diff_vec) / m_vec2f_dot(curr_vec, next_vec);
        MVec2f v = m_vec2f_add(curr.start, m_vec2f_mul_v(curr_vec, j));

        vec_push(stroke, m_edge_vec2(curr.end, v));
        vec_push(stroke, m_edge_vec2(v, next.start));
    }
    else if (join == GFX_STROKE_JOIN_ROUND)
    {
        int divisions = 16;

        MVec2f center = m_vec2f_div_v(m_vec2f_add(curr.end, next.start), 2);

        float curr_angle = m_vec2f_angle(m_vec2f_sub(curr.end, curr.start)).v;
        float next_angle = m_vec2f_angle(m_vec2f_sub(next.start, next.end)).v;

        MVec2f p = curr.end;
        for (int i = 0; i < divisions; i++)
        {
            float angle = curr_angle + i * (next_angle - curr_angle) / (divisions - 1);
            MVec2f v = m_vec2f(center.x + cos(angle) * dist, center.y + sin(angle) * dist);
            vec_push(stroke, m_edge_vec2(p, v));
            p = v;
        }
    }
    else
    {
        vec_push(stroke, m_edge_vec2(curr.end, next.start));
    }
}

void gfx_stroke_apply(GfxStroke style, MEdges *path, MEdges *stroke)
{
    float outer_dist, inner_dist;

    if (style.pos == GFX_STOKE_OUTSIDE)
    {
        outer_dist = style.width;
        inner_dist = 0;
    }
    else if (style.pos == GFX_STOKE_INSIDE)
    {
        outer_dist = 0;
        inner_dist = style.width;
    }
    else
    {
        outer_dist = style.width / 2;
        inner_dist = -style.width / 2;
    }

    for (int i = 0; i < vec_len(path); i++)
    {
        MEdge curr = vec_at(path, i);

        MEdge outer_curr = m_edge_parallel(curr, outer_dist);
        MEdge inner_curr = m_edge_parallel(curr, inner_dist);

        MEdge next = vec_at(path, (i + 1) % vec_len(path));

        MEdge outer_next = m_edge_parallel(next, outer_dist);
        MEdge inner_next = m_edge_parallel(next, inner_dist);

        vec_push(stroke, outer_curr);
        vec_push(stroke, m_edge_swap(inner_curr));

        vec_push(stroke, m_edge_swap(m_edge_vec2(inner_curr.end, inner_next.start)));
        gfx_stroke_join(style.join, stroke, outer_curr, outer_next, outer_dist);
    }
}
