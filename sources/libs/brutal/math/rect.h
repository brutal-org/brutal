#pragma once

#include <brutal/base.h>
#include <brutal/math/vec2.h>

#define Rect(T) \
    struct      \
    {           \
        T x;    \
        T y;    \
        T w;    \
        T h;    \
    }

typedef Rect(int) Recti;
typedef Rect(float) Rectf;

#define rect_create(T, x, y, w, h) \
    ((T){(x), (y), (w), (h)})

#define rect$(T, RECT) \
    rect_create(T, (RECT).x, (RECT).y, (RECT).w, (RECT).h)

#define rect_collide(RECTA, RECTB) (              \
    {                                             \
        bool _result = false;                     \
        AutoType _rect_a = (RECTA);               \
        AutoType _rect_b = (RECTB);               \
        if (_rect_a.x + _rect_a.w >= _rect_b.x && \
            _rect_a.x <= _rect_b.x + _rect_b.w && \
            _rect_a.y + _rect_b.h >= _rect_b.y && \
            _rect_a.y <= _rect_b.y + _rect_b.h)   \
        {                                         \
            result = true;                        \
        }                                         \
        _result;                                  \
    })

#define rect_collide_point(RECT, VEC) (      \
    {                                        \
        bool _result = false;                \
        AutoType _rect = (RECTA);            \
        AutoType _point = (VEC);             \
        if (_rect.x + _rect.w <= _point.x && \
            _rect.x >= _point.x &&           \
            _rect.y + _rect.h <= _point.y && \
            _rect.y >= _point.y)             \
        {                                    \
            result = true;                   \
        }                                    \
        _result;                             \
    })

#define rect_top(RECT) ((RECT).y)

#define rect_bottom(RECT) ((RECT).y + (RECT).h)

#define rect_left(RECT) ((RECT).x)

#define rect_right(RECT) ((RECT).x + (RECT).w)
