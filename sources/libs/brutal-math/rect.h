#pragma once

#include <brutal-base>

#include "vec2.h"

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
        MVec2f pos;
        MVec2f size;
    };

    float elements[4];
} MRectf;

static inline MRectf m_rectf(float x, float y, float z, float w)
{
    return (MRectf){{x, y, z, w}};
}

#define m_rectf_top(RECT) ((RECT).y)

#define m_rectf_bottom(RECT) ((RECT).y + (RECT).height)

#define m_rectf_start(RECT) ((RECT).x)

#define m_rectf_end(RECT) ((RECT).x + (RECT).width)

static inline MVec2f m_rectf_center(MRectf rect)
{
    return m_vec2f(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

static inline bool m_rectf_empty(MRectf rect)
{
    return (int)rect.width == 0 || (int)rect.height == 0;
}

static inline MRectf m_rectf_move(MRectf self, MVec2f offset)
{
    return m_rectf(
        self.x + offset.x,
        self.y + offset.y,
        self.width,
        self.height);
}

static inline bool m_rectf_collide_rect(MRectf recta, MRectf rectb)
{
    return recta.x < rectb.x + rectb.width &&
           recta.x + recta.width > rectb.x &&
           recta.y < rectb.y + rectb.height &&
           recta.height + recta.y > rectb.y;
}

static inline bool m_rectf_contains_rect(MRectf recta, MRectf rectb)
{
    return recta.x <= rectb.x &&
           recta.x + recta.width >= rectb.x + rectb.width &&
           recta.y <= rectb.y &&
           recta.y + recta.height >= rectb.y + rectb.height;
}

static inline bool m_rectf_collide_point(MRectf rect, MVec2f p)
{
    return rect.x + rect.width > p.x &&
           rect.x <= p.x &&
           rect.y + rect.height > p.y &&
           rect.y <= p.y;
}

static inline MRectf m_rectf_from_points(MVec2f a, MVec2f b)
{
    return (MRectf){{
        m_min(a.x, b.x),
        m_min(a.y, b.y),
        m_max(a.x, b.x) - m_min(a.x, b.x),
        m_max(a.y, b.y) - m_min(a.y, b.y),
    }};
}

static inline MRectf m_rectf_merge_rect(MRectf recta, MRectf rectb)
{
    MVec2f p0 = m_vec2f(
        m_min(recta.x, rectb.x),
        m_min(recta.y, rectb.y));

    MVec2f p1 = m_vec2f(
        m_max(recta.x + recta.width, rectb.x + rectb.width),
        m_max(recta.y + recta.height, rectb.y + rectb.height));

    return m_rectf_from_points(p0, p1);
}

static inline MRectf m_rectf_clip_rect(MRectf recta, MRectf rectb)
{
    if (!m_rectf_collide_rect(recta, rectb))
    {
        return (MRectf){};
    }

    MVec2f p0 = m_vec2f(
        m_max(recta.x, rectb.x),
        m_max(recta.y, rectb.y));

    MVec2f p1 = m_vec2f(
        m_min(recta.x + recta.width, rectb.x + rectb.width),
        m_min(recta.y + recta.height, rectb.y + rectb.height));

    return m_rectf_from_points(p0, p1);
}

static inline MRectf m_rectf_cover(MRectf container, MRectf overlay)
{
    float scale = m_vec2f_comp_max(m_vec2f_div(container.size, overlay.size));
    MRectf result = m_rectf(0, 0, overlay.width * scale, overlay.height * scale);
    result.pos = m_vec2f_sub(m_rectf_center(container), m_rectf_center(result));
    return result;
}

static inline MRectf m_rectf_fit(MRectf container, MRectf overlay)
{
    float scale = m_vec2f_comp_min(m_vec2f_div(container.size, overlay.size));
    MRectf result = m_rectf(0, 0, overlay.width * scale, overlay.height * scale);
    result.pos = m_vec2f_sub(m_rectf_center(container), m_rectf_center(result));
    return result;
}

static inline void m_rectf_substract(MRectf rect, MRectf sub, MRectf t[4])
{
    if (m_rectf_collide_rect(rect, sub))
    {
        sub = m_rectf_clip_rect(rect, sub);

        t[0] = m_rectf(rect.x, rect.y, sub.x - rect.x, rect.height);
        t[1] = m_rectf(sub.x + sub.width, rect.y, rect.x + rect.width - sub.x - sub.width, rect.height);
        t[2] = m_rectf(t[0].x + t[0].width, rect.y, t[1].x - t[0].x - t[0].width, sub.y - rect.y);
        t[3] = m_rectf(t[0].x + t[0].width, sub.y + sub.height, t[1].x - t[0].x - t[0].width, rect.y + rect.height - sub.y - sub.height);
    }
    else
    {
        t[0] = rect;
        t[1] = (MRectf){};
        t[2] = (MRectf){};
        t[3] = (MRectf){};
    }
}

static inline MVec2f m_rectf_clamp_vec2(MRectf rect, MVec2f vec)
{
    return m_vec2f(m_clamp(vec.x, m_rectf_start(rect), m_rectf_end(rect)),
                   m_clamp(vec.y, m_rectf_top(rect), m_rectf_bottom(rect)));
}
