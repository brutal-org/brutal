#pragma once

#include <brutal/math/angle.h>
#include <math.h>

typedef struct
{
    float x;
    float y;
} Vec2;

#define vec_create(T, x, y) \
    ((T){(x), (y)})

#define vec$(T, VEC) \
    vec_create((T), (VEC).x, (VEC).y)

#define vec2_add(a, b) (    \
    {                       \
        AutoType __v = (a); \
        __v.x += (b).x;     \
        __v.y += (b).y;     \
        __v;                \
    })

#define vec2_add_v(a, v) (  \
    {                       \
        AutoType __v = (a); \
        __v.x += v;         \
        __v.y += v;         \
        __v;                \
    })

#define vec2_sub(a, b) (    \
    {                       \
        AutoType __v = (a); \
        __v.x -= (b).x;     \
        __v.y -= (b).y;     \
        __v;                \
    })

#define vec2_sub_v(a, v) (  \
    {                       \
        AutoType __v = (a); \
        __v.x -= v;         \
        __v.y -= v;         \
        __v;                \
    })

#define vec2_mul(a, b) (    \
    {                       \
        AutoType __v = (a); \
        __v.x *= (b).x;     \
        __v.y *= (b).y;     \
        __v;                \
    })

#define vec2_mul_v(a, v) (  \
    {                       \
        AutoType __v = (a); \
        __v.x *= v;         \
        __v.y *= v;         \
        __v;                \
    })

#define vec2_div(a, b) (    \
    {                       \
        AutoType __v = (a); \
        __v.x /= (b).x;     \
        __v.y /= (b).y;     \
        __v;                \
    })

#define vec2_div_v(a, v) (  \
    {                       \
        AutoType __v = (a); \
        __v.x /= v;         \
        __v.y /= v;         \
        __v;                \
    })

#define vec2_len_squared(a) \
    ((a).x * (a).x + (a).y * (a).y)

#define vec2_len(a) \
    sqrt(vec2_len_squared(a))

#define vec2_distance(a, b) \
    vec2_len(vec2_sub(b, a))

#define vec2_angle(a) \
    deg2rad(atan2f((a).y, (a).x))
