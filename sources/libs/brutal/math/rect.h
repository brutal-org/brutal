#pragma once

#include <brutal/base.h>
#include <brutal/math/vec2.h>

typedef union
{

    struct
    {
        float x;
        float y;
        float width;
        float height;
    };

    struct
    {
        MVec2 pos;
        MVec2 size;
    };

    float elements[4];
} MRect;

static inline MRect m_rect(float x, float y, float z, float w)
{
    return (MRect){{x, y, z, w}};
}

#define m_rect_top(RECT) ((RECT).y)

#define m_rect_bottom(RECT) ((RECT).y + (RECT).height)

#define m_rect_start(RECT) ((RECT).x)

#define m_rect_end(RECT) ((RECT).x + (RECT).width)

static inline MVec2 m_rect_center(MRect rect)
{
    return m_vec2(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

static inline bool m_rect_empty(MRect rect)
{
    return (int)rect.width == 0 || (int)rect.height == 0;
}

static inline bool m_rect_collide_rect(MRect recta, MRect rectb)
{
    return recta.x < rectb.x + rectb.width &&
           recta.x + recta.width > rectb.x &&
           recta.y < rectb.y + rectb.height &&
           recta.height + recta.y > rectb.y;
}

static inline bool m_rect_collide_point(MRect rect, MVec2 p)
{
    return rect.x + rect.width > p.x &&
           rect.x <= p.x &&
           rect.y + rect.height > p.y &&
           rect.y <= p.y;
}

static inline MRect m_rect_from_points(MVec2 a, MVec2 b)
{
    return (MRect){{
        m_min(a.x, b.x),
        m_min(a.y, b.y),
        m_max(a.x, b.x) - m_min(a.x, b.x),
        m_max(a.y, b.y) - m_min(a.y, b.y),
    }};
}

static inline MRect m_rect_merge_rect(MRect recta, MRect rectb)
{
    MVec2 p0 = m_vec2(
        m_min(recta.x, rectb.x),
        m_min(recta.y, rectb.y));

    MVec2 p1 = m_vec2(
        m_max(recta.x + recta.width, rectb.x + rectb.width),
        m_max(recta.y + recta.height, rectb.y + rectb.height));

    return m_rect_from_points(p0, p1);
}

static inline MRect m_rect_clip_rect(MRect recta, MRect rectb)
{
    if (!m_rect_collide_rect(recta, rectb))
    {
        return (MRect){};
    }

    MVec2 p0 = m_vec2(
        m_max(recta.x, rectb.x),
        m_max(recta.y, rectb.y));

    MVec2 p1 = m_vec2(
        m_min(recta.x + recta.width, rectb.x + rectb.width),
        m_min(recta.y + recta.height, rectb.y + rectb.height));

    return m_rect_from_points(p0, p1);
}

static inline MRect m_rect_cover(MRect container, MRect overlay)
{
    float scale = m_vec2_max(m_vec2_div(container.size, overlay.size));
    MRect result = m_rect(0, 0, overlay.width * scale, overlay.height * scale);
    result.pos = m_vec2_sub(m_rect_center(container), m_rect_center(result));
    return result;
}

static inline MRect m_rect_fit(MRect container, MRect overlay)
{
    float scale = m_vec2_min(m_vec2_div(container.size, overlay.size));
    MRect result = m_rect(0, 0, overlay.width * scale, overlay.height * scale);
    result.pos = m_vec2_sub(m_rect_center(container), m_rect_center(result));
    return result;
}

static inline void m_rect_substract(MRect rect, MRect sub, MRect t[4])
{
    if (m_rect_collide_rect(rect, sub))
    {
        sub = m_rect_clip_rect(rect, sub);

        t[0] = m_rect(rect.x, rect.y, sub.x - rect.x, rect.height);
        t[1] = m_rect(sub.x + sub.width, rect.y, rect.x + rect.width - sub.x - sub.width, rect.height);
        t[2] = m_rect(t[0].x + t[0].width, rect.y, t[1].x - t[0].x - t[0].width, sub.y - rect.y);
        t[3] = m_rect(t[0].x + t[0].width, sub.y + sub.height, t[1].x - t[0].x - t[0].width, rect.y + rect.height - sub.y - sub.height);
    }
    else
    {
        t[0] = rect;
        t[1] = (MRect){};
        t[2] = (MRect){};
        t[3] = (MRect){};
    }
}
