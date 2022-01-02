#pragma once

#include <brutal/math/vec2.h>

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
        MVec2 x;
        MVec2 y;
        MVec2 o;
    };

    float m[6];
} MTrans2;

#define M_TRANS2_IDENTITY \
    (MTrans2) { 1, 0, 0, 1, 0, 0 }

static inline MTrans2 m_trans2(float xx, float xy, float yx, float yy, float ox, float oy)
{
    return (MTrans2){{xx, xy, yx, yy, ox, oy}};
}

static inline MVec2 m_trans2_apply_vector(MTrans2 trans, MVec2 v)
{
    return m_vec2(
        v.x * trans.xx + v.y * trans.yx,
        v.x * trans.xy + v.y * trans.yy);
}

static inline MVec2 m_trans2_apply_point(MTrans2 trans, MVec2 v)
{
    return m_vec2_add(m_trans2_apply_vector(trans, v), trans.o);
}
