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
