#pragma once

#include <brutal/gfx/color.h>
#include <brutal/math/flow.h>
#include <brutal/math/gravity.h>
#include <brutal/math/rect.h>

/* --- Spacing -------------------------------------------------------------- */

typedef struct
{
    float start;
    float top;
    float end;
    float bottom;
} UiSpacing;

static inline MRect ui_spacing_shrink(UiSpacing spacing, MFlow flow, MRect rect)
{
    rect = m_flow_set_start(flow, rect, m_flow_get_start(flow, rect) + spacing.start);
    rect = m_flow_set_top(flow, rect, m_flow_get_top(flow, rect) + spacing.top);
    rect = m_flow_set_end(flow, rect, m_flow_get_end(flow, rect) - spacing.end);
    rect = m_flow_set_bottom(flow, rect, m_flow_get_bottom(flow, rect) - spacing.bottom);

    return rect;
}

static inline MRect ui_spacing_grow(UiSpacing spacing, MFlow flow, MRect rect)
{
    rect = m_flow_set_start(flow, rect, m_flow_get_start(flow, rect) - spacing.start);
    rect = m_flow_set_top(flow, rect, m_flow_get_top(flow, rect) - spacing.top);
    rect = m_flow_set_end(flow, rect, m_flow_get_end(flow, rect) + spacing.end);
    rect = m_flow_set_bottom(flow, rect, m_flow_get_bottom(flow, rect) + spacing.bottom);

    return rect;
}

/* --- Dock ----------------------------------------------------------------- */

typedef enum
{
    UI_DOCK_NONE,

    UI_DOCK_START,
    UI_DOCK_TOP,
    UI_DOCK_END,
    UI_DOCK_BOTTOM,
    UI_DOCK_FILL,
} UiDock;

static inline MFlow ui_dock_to_flow(UiDock dock)
{
    switch (dock)
    {
    default:
    case UI_DOCK_NONE:
    case UI_DOCK_FILL:
        return M_FLOW_LEFT_TO_RIGHT;

    case UI_DOCK_START:
        return M_FLOW_LEFT_TO_RIGHT;

    case UI_DOCK_TOP:
        return M_FLOW_TOP_TO_BOTTOM;

    case UI_DOCK_END:
        return M_FLOW_RIGHT_TO_LEFT;

    case UI_DOCK_BOTTOM:
        return M_FLOW_BOTTOM_TO_TOP;
    }
}

static inline MRect ui_dock_apply(UiDock dock, MFlow flow, MRect content, MRect *container)
{
    flow = m_flow_relative(flow, ui_dock_to_flow(dock));

    switch (dock)
    {
    default:
    case UI_DOCK_NONE:
        break;

    case UI_DOCK_TOP:
    case UI_DOCK_START:
    case UI_DOCK_END:
    case UI_DOCK_BOTTOM:
        content = m_flow_set_origin(flow, content, m_flow_get_origin(flow, *container));
        content = m_flow_set_height(flow, content, m_flow_get_height(flow, *container));
        *container = m_flow_set_start(flow, *container, m_flow_get_end(flow, content));
        break;

    case UI_DOCK_FILL:
        content = *container;
        break;
    }

    return content;
}

/* --- Size ----------------------------------------------------------------- */

typedef struct
{
    MVec2 min;
    MVec2 max;
    bool square;
} UiSize;

/* --- Layouts -------------------------------------------------------------- */

typedef enum
{
    UI_LAYOUT_DOCK,
    UI_LAYOUT_FLEX,
    UI_LAYOUT_GRID,
} UiLayout;

/* --- Style --------------------------------------------------------------- */

typedef struct
{
    UiLayout layout;
    UiSize size;
    UiDock dock;
    UiGravity gravity;
    UiGravity placement;
    UiSpacing padding;
    UiSpacing margin;
    MFlow flow;
    int grow;
} UiStyle;
