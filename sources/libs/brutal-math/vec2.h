#pragma once

#include <math.h>

#include "angle.h"
#include "clamp.h"

typedef union
{
    struct
    {
        int x, y;
    };

    struct
    {
        int width, height;
    };

    struct
    {
        int left, right;
    };

    struct
    {
        int top, bottom;
    };

    int m[2];
} MVec2i;

typedef union
{
    struct
    {
        float x, y;
    };

    struct
    {
        float width, height;
    };

    struct
    {
        float left, right;
    };

    struct
    {
        float top, bottom;
    };

    float m[2];
} MVec2f;

static inline MVec2f m_vec2f(float x, float y)
{
    return (MVec2f){{x, y}};
}

#define M_VEC2_FUNC(NAME, OP)                                    \
                                                                 \
    static inline MVec2f m_vec2f_##NAME(MVec2f lhs, MVec2f rhs)  \
    {                                                            \
        return m_vec2f(lhs.x OP rhs.x, lhs.y OP rhs.y);          \
    }                                                            \
                                                                 \
    static inline MVec2f m_vec2f_##NAME##_v(MVec2f lhs, float v) \
    {                                                            \
        return m_vec2f(lhs.x OP v, lhs.y OP v);                  \
    }

M_VEC2_FUNC(add, +)
M_VEC2_FUNC(sub, -)
M_VEC2_FUNC(mul, *)
M_VEC2_FUNC(div, /)

static inline MVec2f m_vec2f_max(MVec2f lhs, MVec2f rhs)
{
    return m_vec2f(m_max(lhs.x, rhs.x), m_max(lhs.y, rhs.y));
}

static inline MVec2f m_vec2f_min(MVec2f lhs, MVec2f rhs)
{
    return m_vec2f(m_min(lhs.x, rhs.x), m_min(lhs.y, rhs.y));
}

static inline float m_vec2f_comp_max(MVec2f vec)
{
    return m_max(vec.x, vec.y);
}

static inline float m_vec2f_comp_min(MVec2f vec)
{
    return m_max(vec.x, vec.y);
}

static inline float m_vec2f_len_squared(MVec2f vec)
{
    return vec.x * vec.x + vec.y * vec.y;
}

static inline float m_vec2f_len(MVec2f vec)
{
    return sqrtf(m_vec2f_len_squared(vec));
}

static inline float m_vec2f_dist(MVec2f a, MVec2f b)
{
    return m_vec2f_len(m_vec2f_sub(b, a));
}

static inline MVec2f m_vec2f_norm(MVec2f a)
{
    float magn = m_vec2f_len(a);

    if (magn != 0)
    {
        return m_vec2f_div_v(a, magn);
    }
    else
    {
        return m_vec2f(0, 0);
    }
}

static inline float m_vec2f_dot(MVec2f a, MVec2f b)
{
    return a.x * b.x + a.y * b.y;
}

static inline float m_vec2f_angle_with(MVec2f a, MVec2f b)
{
    float r = m_vec2f_dot(m_vec2f_norm(a), m_vec2f_norm(b));
    float sign = (a.x * b.y < a.y * b.x) ? -1.0f : 1.0f;

    return sign * acosf(r);
}

#define m_vec2f_angle(a) \
    m_deg2rad(m_deg(atan2f((a).y, (a).x)))
