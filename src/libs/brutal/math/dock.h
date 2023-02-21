#pragma once

#include "flow.h"
#include "orientation.h"

typedef enum
{
    M_DOCK_NONE,
    M_DOCK_FILL,

    M_DOCK_START,
    M_DOCK_TOP,
    M_DOCK_END,
    M_DOCK_BOTTOM,
} MDock;

MFlow m_dock_to_flow(MDock dock);

MRectf m_dock_apply(MDock dock, MFlow flow, MRectf content, MRectf *container);

MOrientation m_dock_orientation(MDock dock);
