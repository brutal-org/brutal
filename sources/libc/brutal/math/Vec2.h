#pragma once

#define Vec2(T) \
    struct      \
    {           \
        T x;    \
        T y;    \
    }

typedef Vec2(int) Vec2i;
typedef Vec2(float) Vec2f;
