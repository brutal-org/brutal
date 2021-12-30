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

#define m_edge_min_x(EDGE) m_min((EDGE).sx, (EDGE).ex)
#define m_edge_max_x(EDGE) m_max((EDGE).sx, (EDGE).ex)
#define m_edge_min_y(EDGE) m_min((EDGE).sy, (EDGE).ey)
#define m_edge_max_y(EDGE) m_max((EDGE).sy, (EDGE).ey)
