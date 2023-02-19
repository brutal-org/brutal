#include "spacing.h"

MSpacing m_spacing_all(float value)
{
    return (MSpacing){value, value, value, value};
}

MRectf m_spacing_shrink(MSpacing spacing, MFlow flow, MRectf rect)
{
    rect = m_flow_set_start(flow, rect, m_flow_get_start(flow, rect) + spacing.start);
    rect = m_flow_set_top(flow, rect, m_flow_get_top(flow, rect) + spacing.top);
    rect = m_flow_set_end(flow, rect, m_flow_get_end(flow, rect) - spacing.end);
    rect = m_flow_set_bottom(flow, rect, m_flow_get_bottom(flow, rect) - spacing.bottom);

    return rect;
}

MRectf m_spacing_grow(MSpacing spacing, MFlow flow, MRectf rect)
{
    rect = m_flow_set_start(flow, rect, m_flow_get_start(flow, rect) - spacing.start);
    rect = m_flow_set_top(flow, rect, m_flow_get_top(flow, rect) - spacing.top);
    rect = m_flow_set_end(flow, rect, m_flow_get_end(flow, rect) + spacing.end);
    rect = m_flow_set_bottom(flow, rect, m_flow_get_bottom(flow, rect) + spacing.bottom);

    return rect;
}
