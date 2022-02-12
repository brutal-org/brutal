#pragma once

#include <brutal/math/flow.h>

typedef enum
{
    M_DOCK_NONE,

    M_DOCK_START,
    M_DOCK_TOP,
    M_DOCK_END,
    M_DOCK_BOTTOM,
    M_DOCK_FILL,
} MDock;

MFlow m_dock_to_flow(MDock dock);

MRect m_dock_apply(MDock dock, MFlow flow, MRect content, MRect *container);
