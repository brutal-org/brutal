#pragma once

#include <brutal/math/angle.h>
#include <math.h>

typedef union
{
    struct
    {
        float x;
        float y;
    };

    struct
    {
        float width;
        float height;
    };

    struct
    {
        float left;
        float right;
    };

    float elements[2];
} MVec2;

static inline MVec2 m_vec2(float x, float y)
{
    return (MVec2){{x, y}};
}

#define M_VEC2_FUNC(NAME, OP)                                 \
                                                              \
    static inline MVec2 m_vec2_##NAME(MVec2 lhs, MVec2 rhs)   \
    {                                                         \
        return m_vec2(lhs.x OP rhs.x, lhs.y OP rhs.y);        \
    }                                                         \
                                                              \
    static inline MVec2 m_vec2_##NAME##_v(MVec2 lhs, float v) \
    {                                                         \
        return m_vec2(lhs.x OP v, lhs.y OP v);                \
    }

M_VEC2_FUNC(add, +)
M_VEC2_FUNC(sub, -)
M_VEC2_FUNC(mul, *)
M_VEC2_FUNC(div, /)

static inline float m_vec2_len_squared(MVec2 vec)
{
    return vec.x * vec.x + vec.y * vec.y;
}

static inline float m_vec2_len(MVec2 vec)
{
    return sqrt(m_vec2_len_squared(vec));
}

static inline float m_vec2_dist(MVec2 a, MVec2 b)
{
    return m_vec2_len(m_vec2_sub(b, a));
}

#define m_vec2_angle(a) \
    deg2rad(atan2f((a).y, (a).x))

