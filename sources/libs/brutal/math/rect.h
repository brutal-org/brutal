#pragma once

#include <brutal/base.h>

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

#define rect_collide(RECTA, RECTB)                    \
    (                                                 \
        {                                             \
            bool _result = false;                     \
            auto _rect_a = (RECTA);                   \
            auto _rect_b = (RECTB);                   \
            if (_rect_a.x + _rect_a.w >= _rect_b.x && \
                _rect_a.x <= _rect_b.x + _rect_b.w && \
                _rect_a.y + _rect_b.h >= _rect_b.y && \
                _rect_a.y <= _rect_b.y + _rect_b.h)   \
            {                                         \
                result = true;                        \
            }                                         \
            _result;                                  \
        })
