#pragma once

#include <brutal/math/vec2.h>

#define Edge(T)  \
    struct       \
    {            \
        T start; \
        T end;   \
    }

typedef Edge(Vec2i) Edgei;
typedef Edge(Vec2f) Edgef;

#define edge_create(T, start, end) \
    ((T){start, end})

#define edge_cast(T, from) \
    edge_create(T, (from).start, (from).end)
