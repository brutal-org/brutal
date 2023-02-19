#include "gravity.h"

MRectf m_gravity_apply(MGravity gravity, MFlow flow, MRectf content, MRectf container)
{
    if (gravity == M_GRAVITY_NONE)
    {
        return container;
    }

    if (gravity & M_GRAVITY_COVER)
    {
        content = m_rectf_cover(content, container);
    }

    if (gravity & M_GRAVITY_FIT)
    {
        content = m_rectf_fit(content, container);
    }

    if (gravity & M_GRAVITY_START)
    {
        content = m_flow_set_x(flow, content, m_flow_get_start(flow, container));
    }

    if (gravity & M_GRAVITY_TOP)
    {
        content = m_flow_set_y(flow, content, m_flow_get_top(flow, container));
    }

    if (gravity & M_GRAVITY_END)
    {
        content = m_flow_set_x(flow, content, m_flow_get_end(flow, container) - m_flow_get_width(flow, content));
    }

    if (gravity & M_GRAVITY_BOTTOM)
    {
        content = m_flow_set_y(flow, content, m_flow_get_bottom(flow, container) - m_flow_get_height(flow, content));
    }

    if (gravity & M_GRAVITY_HSTRETCH)
    {
        content = m_flow_set_width(flow, content, m_flow_get_width(flow, container));
    }

    if (gravity & M_GRAVITY_VSTRETCH)
    {
        content = m_flow_set_height(flow, content, m_flow_get_height(flow, container));
    }

    if (gravity & M_GRAVITY_HCENTER)
    {
        content = m_flow_set_x(flow, content, m_flow_get_hcenter(flow, container) - m_flow_get_width(flow, content) / 2);
    }

    if (gravity & M_GRAVITY_VCENTER)
    {
        content = m_flow_set_y(flow, content, m_flow_get_vcenter(flow, container) - m_flow_get_height(flow, content) / 2);
    }

    return content;
}
