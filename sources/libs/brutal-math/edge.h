#pragma once

#include <brutal-ds>
#include <brutal-io>

#include "clamp.h"
#include "rect.h"
#include "vec2.h"

typedef union
{
    struct
    {
        float sx;
        float sy;
        float ex;
        float ey;
    };

    struct
    {
        MVec2f start;
        MVec2f end;
    };
} MEdge;

typedef Vec(MEdge) MEdges;

static inline MEdge m_edge(float sx, float sy, float ex, float ey)
{
    return (MEdge){{sx, sy, ex, ey}};
}

static inline MEdge m_edge_vec2(MVec2f start, MVec2f end)
{
    return (MEdge){.start = start, .end = end};
}

#define m_edge_min_x(EDGE) m_min((EDGE).sx, (EDGE).ex)
#define m_edge_max_x(EDGE) m_max((EDGE).sx, (EDGE).ex)
#define m_edge_min_y(EDGE) m_min((EDGE).sy, (EDGE).ey)
#define m_edge_max_y(EDGE) m_max((EDGE).sy, (EDGE).ey)

static inline MRectf m_edge_bound(MEdge edge)
{
    return m_rectf_from_points(edge.start, edge.end);
}

static inline MRectf m_edges_bound(MEdge const *edges, size_t len)
{
    if (!len)
    {
        return m_rectf(0, 0, 0, 0);
    }

    MRectf result = m_edge_bound(edges[0]);

    for (size_t i = 1; i < len; i++)
    {
        result = m_rectf_merge_rect(result, m_edge_bound(edges[i]));
    }

    return result;
}

static inline float m_edge_vec_dist(MEdge edge, MVec2f vec)
{
    float d = m_vec2f_dist(edge.start, edge.end);

    if (d < 0.01)
    {
        return m_vec2f_dist(edge.start, vec);
    }

    float t = m_vec2f_dot(m_vec2f_sub(vec, edge.start), m_vec2f_sub(edge.end, edge.start));
    t = m_max(0, m_min(1, t / (d * d)));
    MVec2f projection = m_vec2f_add(edge.start, m_vec2f_mul_v(m_vec2f_sub(edge.end, edge.start), t));
    return m_vec2f_dist(vec, projection);
}

static inline MEdge m_edge_parallel(MEdge edge, float offset)
{
    MVec2f d = m_vec2f_sub(edge.end, edge.start);
    float scale = offset / m_vec2f_len(d);
    MVec2f o = m_vec2f(-d.y * scale, d.x * scale);
    MVec2f s = m_vec2f_add(edge.start, o);
    MVec2f e = m_vec2f_add(edge.end, o);

    return m_edge_vec2(s, e);
}

static inline MEdge m_edge_swap(MEdge edge)
{
    return m_edge(edge.ex, edge.ey, edge.sx, edge.sy);
}

static inline float m_edge_len(MEdge edge)
{
    return m_vec2f_len(m_vec2f_sub(edge.end, edge.start));
}
