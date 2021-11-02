#pragma once

#include <brutal/io/buf.h>
#include <brutal/math/min_max.h>
#include <brutal/math/rect.h>
#include <brutal/math/vec2.h>

#define Edge(T) \
    struct      \
    {           \
        T sx;   \
        T sy;   \
        T ex;   \
        T ey;   \
    }

typedef Edge(int) Edgei;
typedef Edge(float) Edgef;

typedef Slice(Edgei) Edgesi;
typedef Slice(Edgef) Edgesf;

typedef InlineBuf(Edgei) InlineEdgesi;
typedef InlineBuf(Edgef) InlineEdgesf;

#define edge_min_y(EDGE) math_min((EDGE).sy, (EDGE).ey)
#define edge_max_y(EDGE) math_max((EDGE).sy, (EDGE).ey)

#define edges_bound(EDGES) (                           \
    {                                                  \
        float minx = 0, miny = 0, maxx = 0, maxy = 0;  \
                                                       \
        for (size_t i = 0; i < (EDGES).len; i++)       \
        {                                              \
            typeof(*edges.buf) e = edges.buf[i];       \
                                                       \
            minx = MIN(minx, e.sx);                    \
            miny = MIN(miny, e.sy);                    \
            maxx = MAX(maxx, e.sx);                    \
            maxy = MAX(maxy, e.sy);                    \
                                                       \
            minx = MIN(minx, e.ex);                    \
            miny = MIN(miny, e.ey);                    \
            maxx = MAX(maxx, e.ex);                    \
            maxy = MAX(maxy, e.ey);                    \
        }                                              \
                                                       \
        (Rectf){minx, miny, maxx - minx, maxy - miny}; \
    })
