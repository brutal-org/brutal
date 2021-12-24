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

#define edge_min_y(EDGE) math_min((EDGE).sy, (EDGE).ey)
#define edge_max_y(EDGE) math_max((EDGE).sy, (EDGE).ey)
