#pragma once

#include <math/math.h>

#define Vec2(T) \
    struct      \
    {           \
        T x;    \
        T y;    \
    }

typedef Vec2(int) Vec2i;
typedef Vec2(float) Vec2f;
typedef Vec2(double) Vec2d;

#define vec_create(T, x, y) \
    ((T){(x), (y)})

#define vec$(T, VEC) \
    vec_create((T), (VEC).x, (VEC).y)

#define vec2_add(a, b) ( \
    {                    \
        auto __v = (a);  \
        __v.x += (b).x;  \
        __v.y += (b).y;  \
        __v;             \
    })

#define vec2_sub(a, b) ( \
    {                    \
        auto __v = (a);  \
        __v.x -= (b).x;  \
        __v.y -= (b).y;  \
        __v;             \
    })

#define vec2_mul(a, b) ( \
    {                    \
        auto __v = (a);  \
        __v.x *= (b).x;  \
        __v.y *= (b).y;  \
        __v;             \
    })

#define vec2_div(a, b) ( \
    {                    \
        auto __v = (a);  \
        __v.x /= (b).x;  \
        __v.y /= (b).y;  \
        __v;             \
    })

#define vec2_distance(a, b) (                                          \
    {                                                                  \
        auto root = pow(((a).x - (b).x), 2) + pow(((a).y - (b).y), 2); \
        root = sqrt(root);                                             \
        root;                                                          \
    })
