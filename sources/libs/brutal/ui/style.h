#pragma once

#include <brutal/gfx/color.h>
#include <brutal/math/flow.h>
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

/* --- Gravity -------------------------------------------------------------- */

#define UI_GRAVITY_START (1 << 0)
#define UI_GRAVITY_TOP (1 << 1)

#define UI_GRAVITY_END (1 << 2)
#define UI_GRAVITY_BOTTOM (1 << 3)

#define UI_GRAVITY_VSTRETCH (1 << 4)
#define UI_GRAVITY_HSTRETCH (1 << 5)

#define UI_GRAVITY_VCENTER (1 << 6)
#define UI_GRAVITY_HCENTER (1 << 7)

#define UI_GRAVITY_VFILL (UI_GRAVITY_VSTRETCH | UI_GRAVITY_START)
#define UI_GRAVITY_HFILL (UI_GRAVITY_HSTRETCH | UI_GRAVITY_TOP)

#define M_GRAVITY_CENTER (M_GRAVITY_HCENTER | M_GRAVITY_VCENTER)
#define M_GRAVITY_FILL (M_GRAVITY_VFILL | M_GRAVITY_HFILL)

typedef uint64_t UiGravity;

static inline MRect ui_gravity_apply(UiGravity gravity, MFlow flow, MRect content, MRect container)
{
    if (gravity & UI_GRAVITY_START)
    {
        content = m_flow_set_start(flow, content, m_flow_get_start(flow, container));
    }

    if (gravity & UI_GRAVITY_TOP)
    {
        content = m_flow_set_top(flow, content, m_flow_get_top(flow, container));
    }

    if (gravity & UI_GRAVITY_END)
    {
        content = m_flow_set_start(flow, content, m_flow_get_end(flow, container) - m_flow_get_width(flow, container));
    }

    if (gravity & UI_GRAVITY_BOTTOM)
    {
        content = m_flow_set_top(flow, content, m_flow_get_bottom(flow, container) - m_flow_get_height(flow, container));
    }

    if (gravity & UI_GRAVITY_VSTRETCH)
    {
        content = m_flow_set_width(flow, content, m_flow_get_width(flow, container));
    }

    if (gravity & UI_GRAVITY_HSTRETCH)
    {
        content = m_flow_set_height(flow, content, m_flow_get_height(flow, container));
    }

    if (gravity & UI_GRAVITY_HCENTER)
    {
        content = m_flow_set_start(flow, content, m_flow_get_hcenter(flow, container) - m_flow_get_width(flow, content) / 2);
    }

    if (gravity & UI_GRAVITY_VCENTER)
    {
        content = m_flow_set_top(flow, content, m_flow_get_vcenter(flow, container) - m_flow_get_height(flow, content) / 2);
    }

    return content;
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
