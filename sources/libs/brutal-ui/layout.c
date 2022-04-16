#include "layout.h"

#include "view.h"

/* --- Compute Size --------------------------------------------------------- */

static MVec2f ui_dock_size(UiView *views[], size_t len)
{
    MVec2f total_size = m_vec2f(0, 0);
    MVec2f current_size = m_vec2f(0, 0);
    MOrientation previous_orientation = M_ORIENTATION_NONE;

    for (size_t i = 0; i < len; i++)
    {
        UiLayout const *child_layout = &views[i]->layout;
        MRectf child_bound = ui_view_size(views[i]);
        MOrientation child_orientation = m_dock_orientation(child_layout->dock);

        if (previous_orientation != child_orientation)
        {
            total_size = m_vec2f_add(total_size, current_size);

            previous_orientation = child_orientation;
            current_size = m_vec2f(0, 0);
        }

        switch (child_orientation)
        {
        case M_ORIENTATION_NONE:
            current_size = m_vec2f_max(current_size, child_bound.size);
            break;

        case M_ORIENTATION_HORIZONTAL:
            current_size.x += child_bound.size.x;
            current_size.y = m_max(current_size.y, child_bound.size.y);
            break;

        case M_ORIENTATION_VERTICAL:
            current_size.x = m_max(current_size.x, child_bound.size.x);
            current_size.y += child_bound.size.y;
            break;

        default:
            panic$("Invalid orientation");
        }
    }

    total_size = m_vec2f_add(total_size, current_size);

    return total_size;
}

static MVec2f ui_flex_size(UiLayout const *self, UiView *views[], size_t len)
{
    MVec2f size = m_vec2f(0, 0);
    MFlow flow = self->flow;

    for (size_t i = 0; i < len; i++)
    {
        UiLayout const *child_layout = &views[i]->layout;
        MRectf child_bound = ui_view_size(views[i]);

        if (child_layout->dock == M_DOCK_START ||
            child_layout->dock == M_DOCK_END ||
            child_layout->dock == M_DOCK_FILL)
        {
            size.x += m_flow_get_width(flow, child_bound);
        }

        if (child_layout->dock == M_DOCK_TOP ||
            child_layout->dock == M_DOCK_BOTTOM ||
            child_layout->dock == M_DOCK_FILL)
        {
            size.y = m_max(m_flow_get_height(flow, child_bound), size.y);
        }
    }

    size.y += self->gaps.x * (len - 1);

    return size;
}

static MVec2f ui_max_size(struct _UiView *views[], size_t len)
{
    MVec2f result = m_vec2f(0, 0);

    for (size_t i = 0; i < len; i++)
    {
        MVec2f size = ui_view_size(views[i]).size;
        result.x = m_max(result.x, size.x);
        result.y = m_max(result.y, size.y);
    }

    return result;
}

MVec2f ui_layout_size(UiLayout const *self, struct _UiView *views[], size_t len)
{
    switch (self->type)
    {
    case UI_LAYOUT_DOCK:
        return ui_dock_size(views, len);

    case UI_LAYOUT_FLEX:
        return ui_flex_size(self, views, len);

    default:
        return ui_max_size(views, len);
    }
}

/* --- Layout --------------------------------------------------------------- */

static void ui_dock_run(UiLayout const *self, MRectf container, UiView *views[], size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        MRectf content = ui_view_size(child);
        content = m_dock_apply(child->layout.dock, self->flow, content, &container);
        ui_view_place(child, content);
    }
}

static void ui_flex_run(UiLayout const *self, MRectf container, UiView *views[], size_t len)
{
    MFlow flow = self->flow;

    float grows = 0;
    float total = 0;

    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        UiLayout child_layout = child->layout;

        if (child_layout.grow > 0.01)
        {
            grows += child_layout.grow;
        }
        else
        {
            total += m_flow_get_width(flow, ui_view_size(child));
        }
    }

    float all = m_flow_get_width(flow, container) - self->gaps.x * (len - 1);
    float grow_total = m_max(0, all - total);
    float grow_unit = (grow_total) / m_max(1, grows);
    float start = m_flow_get_start(flow, container);

    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        UiLayout child_layout = child->layout;
        MRectf child_bound = {};

        if (child_layout.grow > 0.01)
        {
            child_bound = m_flow_set_start(flow, child_bound, start);
            child_bound = m_flow_set_width(flow, child_bound, grow_unit * child_layout.grow);
        }
        else
        {
            child_bound = ui_view_size(child);
            child_bound = m_flow_set_x(flow, child_bound, start);
        }

        child_bound = m_flow_set_y(flow, child_bound, m_flow_get_top(flow, container));
        child_bound = m_flow_set_height(flow, child_bound, m_flow_get_height(flow, container));

        ui_view_place(child, child_bound);
        start += m_flow_get_width(flow, child_bound) + self->gaps.x;
    }
}

void ui_layout_run(UiLayout const *self, MRectf container, UiView *views[], size_t len)
{
    switch (self->type)
    {
    case UI_LAYOUT_DOCK:
        ui_dock_run(self, container, views, len);
        break;

    case UI_LAYOUT_FLEX:
        ui_flex_run(self, container, views, len);
        break;

    default:
        break;
    }
}
