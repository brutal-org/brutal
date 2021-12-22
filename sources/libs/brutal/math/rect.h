#pragma once

#include <brutal/base.h>
#include <brutal/math/vec2.h>

typedef struct
{
    float x;
    float y;
    float w;
    float h;
} Rect;

#define rect_top(RECT) ((RECT).y)

#define rect_bottom(RECT) ((RECT).y + (RECT).h)

#define rect_left(RECT) ((RECT).x)

#define rect_right(RECT) ((RECT).x + (RECT).w)

static inline bool rect_collide_rect(Rect recta, Rect rectb)
{
    return recta.x + recta.w >= rectb.x &&
           recta.x <= rectb.x + rectb.w &&
           recta.y + rectb.h >= rectb.y &&
           recta.y <= rectb.y + rectb.h;
}

static inline bool rect_collide_point(Rect rect, Vec2 p)
{
    return rect.x + rect.w <= p.x &&
           rect.x >= p.x &&
           rect.y + rect.h <= p.y &&
           rect.y >= p.y;
}

static inline Rect rect_from_points(Vec2 a, Vec2 b)
{
    return (Rect){
        MIN(a.x, b.x),
        MIN(a.y, b.y),
        MAX(a.x, b.x),
        MAX(a.y, b.y),
    };
}

static inline Rect rect_merge_rect(Rect recta, Rect rectb)
{
    Vec2 p0 = {
        MIN(recta.x, rectb.x),
        MIN(recta.y, rectb.y),
    };

    Vec2 p1 = {
        MAX(recta.x + recta.w, rectb.x + rectb.w),
        MAX(recta.y + recta.h, rectb.y + rectb.h),
    };

    return rect_from_points(p0, p1);
}

static inline Rect rect_clip_rect(Rect recta, Rect rectb)
{
    if (!rect_collide_rect(recta, rectb))
    {
        return (Rect){};
    }

    Vec2 p0 = {
        MAX(recta.x, rectb.x),
        MAX(recta.y, rectb.y),
    };

    Vec2 p1 = {
        MIN(recta.x + recta.w, rectb.x + rectb.w),
        MIN(recta.y + recta.h, rectb.y + rectb.h),
    };

    return rect_from_points(p0, p1);
}
