#pragma once

#define Vec2(T) \
    struct      \
    {           \
        T x;    \
        T y;    \
    }

typedef Vec2(int) Vec2i;
typedef Vec2(float) Vec2f;

#define vec_create(T, x, y) \
    ((T){(x), (y)})

#define vec$(T, VEC) \
    vec_create((T), (VEC).x, (VEC).y)
