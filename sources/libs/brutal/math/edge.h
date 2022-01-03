#pragma once

#include <brutal/io/buf.h>
#include <brutal/math/clamp.h>
#include <brutal/math/rect.h>
#include <brutal/math/vec2.h>

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
        MVec2 start;
        MVec2 end;
    };
} MEdge;

static inline MEdge m_edge(float sx, float sy, float ex, float ey)
{
    return (MEdge){{sx, sy, ex, ey}};
}

static inline MEdge m_edge_vec2(MVec2 start, MVec2 end)
{
    return (MEdge){.start = start, .end = end};
}

#define m_edge_min_x(EDGE) m_min((EDGE).sx, (EDGE).ex)
#define m_edge_max_x(EDGE) m_max((EDGE).sx, (EDGE).ex)
#define m_edge_min_y(EDGE) m_min((EDGE).sy, (EDGE).ey)
#define m_edge_max_y(EDGE) m_max((EDGE).sy, (EDGE).ey)

static inline MRect m_edge_bound(MEdge edge)
{
    return m_rect_from_points(edge.start, edge.end);
}

static inline MRect m_edges_bound(MEdge const *edges, size_t len)
{
    if (!len)
    {
        return m_rect(0, 0, 0, 0);
    }

    MRect result = m_edge_bound(edges[0]);

    for (size_t i = 1; i < len; i++)
    {
        result = m_rect_merge_rect(result, m_edge_bound(edges[i]));
    }

    return result;
}
