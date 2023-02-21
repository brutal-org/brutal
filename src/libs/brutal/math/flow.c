#include "flow.h"

MFlow m_flow_relative(MFlow parent, MFlow child)
{
    // FIXME: make sure this is correct
    MFlow table[4 * 4] = {
        M_FLOW_LEFT_TO_RIGHT,
        M_FLOW_RIGHT_TO_LEFT,
        M_FLOW_TOP_TO_BOTTOM,
        M_FLOW_BOTTOM_TO_TOP,

        M_FLOW_RIGHT_TO_LEFT,
        M_FLOW_LEFT_TO_RIGHT,
        M_FLOW_TOP_TO_BOTTOM,
        M_FLOW_BOTTOM_TO_TOP,

        M_FLOW_TOP_TO_BOTTOM,
        M_FLOW_BOTTOM_TO_TOP,
        M_FLOW_LEFT_TO_RIGHT,
        M_FLOW_RIGHT_TO_LEFT,

        M_FLOW_BOTTOM_TO_TOP,
        M_FLOW_TOP_TO_BOTTOM,
        M_FLOW_LEFT_TO_RIGHT,
        M_FLOW_RIGHT_TO_LEFT,
    };

    return table[parent * 4 + child];
}

MVec2f m_flow_to_vec(MFlow flow)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
        return m_vec2f(1, 0);

    case M_FLOW_RIGHT_TO_LEFT:
        return m_vec2f(-1, 0);

    case M_FLOW_TOP_TO_BOTTOM:
        return m_vec2f(0, 1);

    case M_FLOW_BOTTOM_TO_TOP:
        return m_vec2f(0, -1);
    }
}

float m_flow_get_start(MFlow flow, MRectf rect)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
        return rect.x;

    case M_FLOW_RIGHT_TO_LEFT:
        return rect.x + rect.width;

    case M_FLOW_TOP_TO_BOTTOM:
        return rect.y;

    case M_FLOW_BOTTOM_TO_TOP:
        return rect.y + rect.height;
    }
}

float m_flow_get_end(MFlow flow, MRectf rect)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
        return rect.x + rect.width;

    case M_FLOW_RIGHT_TO_LEFT:
        return rect.x;

    case M_FLOW_TOP_TO_BOTTOM:
        return rect.y + rect.height;

    case M_FLOW_BOTTOM_TO_TOP:
        return rect.y;
    }
}

float m_flow_get_top(MFlow flow, MRectf rect)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        return rect.y;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        return rect.x;
    }
}

float m_flow_get_bottom(MFlow flow, MRectf rect)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        return rect.y + rect.height;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        return rect.x + rect.width;
    }
}

float m_flow_get_width(MFlow flow, MRectf rect)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        return rect.width;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        return rect.height;
    }
}

float m_flow_get_height(MFlow flow, MRectf rect)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        return rect.height;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        return rect.width;
    }
}

MVec2f m_flow_get_origin(MFlow flow, MRectf rect)
{
    return m_vec2f(
        m_flow_get_start(flow, rect),
        m_flow_get_top(flow, rect));
}

float m_flow_get_hcenter(MFlow flow, MRectf rect)
{
    return (m_flow_get_start(flow, rect) + m_flow_get_end(flow, rect)) / 2;
}

float m_flow_get_vcenter(MFlow flow, MRectf rect)
{
    return (m_flow_get_top(flow, rect) + m_flow_get_bottom(flow, rect)) / 2;
}

MRectf m_flow_set_start(MFlow flow, MRectf rect, float value)
{
    float d = value - m_flow_get_start(flow, rect);

    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
        rect.x += d;
        rect.width -= d;
        return rect;

    case M_FLOW_RIGHT_TO_LEFT:
        rect.width += d;
        return rect;

    case M_FLOW_TOP_TO_BOTTOM:
        rect.y += d;
        rect.height -= d;
        return rect;

    case M_FLOW_BOTTOM_TO_TOP:
        rect.height += d;
        return rect;
    }
}

MRectf m_flow_set_x(MFlow flow, MRectf rect, float value)
{
    float d = value - m_flow_get_start(flow, rect);

    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        rect.x += d;
        return rect;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        rect.y += d;
        return rect;
    }
}

MRectf m_flow_set_end(MFlow flow, MRectf rect, float value)
{
    float d = value - m_flow_get_end(flow, rect);

    switch (flow)
    {
    case M_FLOW_RIGHT_TO_LEFT:
        rect.x += d;
        [[fallthrough]];

    default:
    case M_FLOW_LEFT_TO_RIGHT:
        rect.width += d;
        return rect;

    case M_FLOW_BOTTOM_TO_TOP:
        rect.y += d;
        [[fallthrough]];

    case M_FLOW_TOP_TO_BOTTOM:
        rect.height += d;
        return rect;
    }
}

MRectf m_flow_set_top(MFlow flow, MRectf rect, float value)
{
    float d = value - m_flow_get_top(flow, rect);

    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        rect.y += d;
        rect.height -= d;
        return rect;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        rect.x += d;
        rect.width -= d;
        return rect;
    }
}

MRectf m_flow_set_y(MFlow flow, MRectf rect, float value)
{
    float d = value - m_flow_get_top(flow, rect);

    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        rect.y += d;
        return rect;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        rect.x += d;
        return rect;
    }
}

MRectf m_flow_set_bottom(MFlow flow, MRectf rect, float value)
{
    float d = value - m_flow_get_bottom(flow, rect);

    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        rect.height += d;
        return rect;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        rect.width += d;
        return rect;
    }
}

MRectf m_flow_set_origin(MFlow flow, MRectf rect, MVec2f value)
{
    rect = m_flow_set_x(flow, rect, value.x);
    rect = m_flow_set_y(flow, rect, value.y);
    return rect;
}

MRectf m_flow_set_width(MFlow flow, MRectf rect, float value)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        rect.width = value;
        return rect;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        rect.height = value;
        return rect;
    }
}

MRectf m_flow_set_height(MFlow flow, MRectf rect, float value)
{
    switch (flow)
    {
    default:
    case M_FLOW_LEFT_TO_RIGHT:
    case M_FLOW_RIGHT_TO_LEFT:
        rect.height = value;
        return rect;

    case M_FLOW_TOP_TO_BOTTOM:
    case M_FLOW_BOTTOM_TO_TOP:
        rect.width = value;
        return rect;
    }
}
