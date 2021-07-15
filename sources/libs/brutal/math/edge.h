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
