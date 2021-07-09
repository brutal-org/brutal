#pragma once

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

#define rect_cast(T, RECT) \
    ((T){(RECT).x, (RECT).y, (RECT).w, (RECT).h})
