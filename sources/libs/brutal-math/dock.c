#include "dock.h"

MFlow m_dock_to_flow(MDock dock)
{
    switch (dock)
    {
    default:
    case M_DOCK_NONE:
    case M_DOCK_FILL:
        return M_FLOW_LEFT_TO_RIGHT;

    case M_DOCK_START:
        return M_FLOW_LEFT_TO_RIGHT;

    case M_DOCK_TOP:
        return M_FLOW_TOP_TO_BOTTOM;

    case M_DOCK_END:
        return M_FLOW_RIGHT_TO_LEFT;

    case M_DOCK_BOTTOM:
        return M_FLOW_BOTTOM_TO_TOP;
    }
}

MRectf m_dock_apply(MDock dock, MFlow flow, MRectf content, MRectf *container)
{
    flow = m_flow_relative(flow, m_dock_to_flow(dock));

    switch (dock)
    {
    case M_DOCK_NONE:
        break;

    case M_DOCK_FILL:
        content = *container;
        break;

    default:
        content = m_flow_set_origin(flow, content, m_flow_get_origin(flow, *container));
        content = m_flow_set_height(flow, content, m_flow_get_height(flow, *container));
        *container = m_flow_set_start(flow, *container, m_flow_get_end(flow, content));
        break;
    }

    return content;
}

MOrientation m_dock_orientation(MDock dock)
{
    switch (dock)
    {
    default:
    case M_DOCK_NONE:
    case M_DOCK_FILL:
        return M_ORIENTATION_NONE;

    case M_DOCK_START:
        return M_ORIENTATION_HORIZONTAL;

    case M_DOCK_TOP:
        return M_ORIENTATION_VERTICAL;

    case M_DOCK_END:
        return M_ORIENTATION_HORIZONTAL;

    case M_DOCK_BOTTOM:
        return M_ORIENTATION_VERTICAL;
    }
}
