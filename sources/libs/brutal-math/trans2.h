#pragma once

#include <math.h>

#include "vec2.h"

typedef union
{
    struct
    {
        float xx;
        float xy;
        float yx;
        float yy;

        float ox;
        float oy;
    };

    struct
    {
        MVec2f x;
        MVec2f y;
        MVec2f o;
    };

    float m[6];
} MTrans2;

static inline MTrans2 m_trans2(float xx, float xy, float yx, float yy, float ox, float oy)
{
    return (MTrans2){{xx, xy, yx, yy, ox, oy}};
}

static inline MTrans2 m_trans2_identity(void)
{
    return m_trans2(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
}

static inline MTrans2 m_trans2_rotate(float r)
{
    float cs = cosf(r);
    float sn = sinf(r);

    return m_trans2(cs, sn, -sn, cs, 0.0f, 0.0f);
}

static inline MTrans2 m_trans2_skew(MVec2f vec)
{
    return m_trans2(
        1.0f,
        tanf(vec.y),
        tanf(vec.x),
        1.0f,
        0.0f,
        0.0f);
}

static inline MTrans2 m_trans2_skewx(float x)
{
    return m_trans2(
        1.0f,
        0.0f,
        tanf(x),
        1.0f,
        0.0f,
        0.0f);
}

static inline MTrans2 m_trans2_skewy(float y)
{
    return m_trans2(
        1.0f,
        tanf(y),
        0.0f,
        1.0f,
        0.0f,
        0.0f);
}

static inline MTrans2 m_trans2_scale(float s)
{
    return m_trans2(s, 0.0f, 0.0f, s, 0, 0);
}

static inline MTrans2 m_trans2_scalex(float x)
{
    return m_trans2(x, 0.0f, 0.0f, 1, 0, 0);
}

static inline MTrans2 m_trans2_scaley(float y)
{
    return m_trans2(1, 0.0f, 0.0f, y, 0, 0);
}

static inline MTrans2 m_trans2_transale(MVec2f vec)
{
    return m_trans2(1.0f, 0.0f, 0.0f, 1.0f, vec.x, vec.y);
}

static inline bool m_trans2_is_aa(MTrans2 trans)
{
    return trans.xx == 1.0f &&
           trans.xy == 0.0f &&
           trans.yx == 0.0f &&
           trans.yy == 1.0f;
}

static inline bool m_trans2_is_identity(MTrans2 trans)
{
    return m_trans2_is_aa(trans) &&
           trans.ox == 0.0f &&
           trans.oy == 0.0f;
}

static inline MVec2f m_trans2_apply_vector(MTrans2 trans, MVec2f v)
{
    return m_vec2f(
        v.x * trans.xx + v.y * trans.yx,
        v.x * trans.xy + v.y * trans.yy);
}

static inline MVec2f m_trans2_apply_point(MTrans2 trans, MVec2f v)
{
    return m_vec2f_add(m_trans2_apply_vector(trans, v), trans.o);
}

static inline MTrans2 m_trans_mult(MTrans2 lhs, MTrans2 rhs)
{
    return m_trans2(
        lhs.m[0] * rhs.m[0] + lhs.m[1] * rhs.m[2],
        lhs.m[0] * rhs.m[1] + lhs.m[1] * rhs.m[3],
        lhs.m[2] * rhs.m[0] + lhs.m[3] * rhs.m[2],
        lhs.m[2] * rhs.m[1] + lhs.m[3] * rhs.m[3],
        lhs.m[4] * rhs.m[1] + lhs.m[5] * rhs.m[3] + rhs.m[5],
        lhs.m[4] * rhs.m[0] + lhs.m[5] * rhs.m[2] + rhs.m[4]);
}
