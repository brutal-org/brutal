#include <brutal/alloc/global.h>
#include <brutal/ui/view.h>
#include <brutal/ui/win.h>

/* --- Lifecycle ------------------------------------------------------------ */

void ui_view_init(UiView *self)
{
    vec_init(&self->children, alloc_global());
    self->bound = m_rect(0, 0, 100, 100);
    self->flags = UI_VIEW_ENABLED | UI_VIEW_VISIBLE;
}

void ui_view_ref(UiView *self)
{
    self->refcount++;
}

void ui_view_deref(UiView *self)
{
    self->refcount--;

    if (self->refcount > 0)
    {
        return;
    }

    if (self->deinit)
    {
        self->deinit(self);
    }

    vec_deinit(&self->children);
    alloc_free(alloc_global(), self);
}

/* --- Childrens ------------------------------------------------------------ */

void ui_view_mount(UiView *self, UiView *child)
{
    ui_view_ref(child);
    ui_view_mounted_to_view(child, self);
    vec_push(&self->children, child);
}

void ui_view_umount(UiView *self, UiView *child)
{
    vec_remove(&self->children, child);
    ui_view_unmounted(child);
    ui_view_deref(child);
}

void ui_view_mounted_to_view(UiView *self, UiView *parent)
{
    assert_truth(self->parent == nullptr && self->window == nullptr);
    self->parent = parent;
}

void ui_view_mounted_to_window(UiView *self, struct _UiWin *window)
{
    assert_truth(self->parent == nullptr && self->window == nullptr);
    self->window = window;
}

void ui_view_unmounted(UiView *self)
{
    assert_truth(self->parent != nullptr || self->window != nullptr);
    self->parent = nullptr;
    self->window = nullptr;
}

/* --- Properties ----------------------------------------------------------- */

MVec2 ui_view_orgin(UiView *self)
{
    return m_vec2_add(self->scroll, self->bound.pos);
}

MRect ui_view_bound(UiView *self)
{
    return self->bound;
}

MRect ui_view_content(UiView *self)
{
    return ui_spacing_shrink(self->style.padding, self->style.flow, ui_view_container(self));
}

MRect ui_view_container(UiView *self)
{
    MRect rect = self->bound;
    rect.pos = m_vec2(0, 0);
    return rect;
}

void ui_view_resize(UiView *self, MRect rect)
{
    self->bound = rect;
}

void ui_view_style(UiView *self, UiStyle style)
{
    self->style = style;
    ui_view_should_repaint(self);
}

struct _UiWin *ui_view_window(UiView *self)
{
    if (self->window)
    {
        return self->window;
    }

    if (self->parent)
    {
        return ui_view_window(self->parent);
    }

    return nullptr;
}

/* --- Paint ---------------------------------------------------------------- */

void ui_view_should_repaint(UiView *self)
{
    ui_view_should_repaint_rect(self, self->bound);
}

void ui_view_should_repaint_rect(UiView *self, MRect dirty)
{
    if (!m_rect_collide_rect(self->bound, dirty))
    {
        return;
    }

    if (self->parent)
    {
        dirty.pos = m_vec2_add(dirty.pos, ui_view_orgin(self->parent));
        ui_view_should_repaint_rect(self->parent, dirty);
    }

    if (self->window)
    {
        ui_win_should_repaint_rect(self->window, dirty);
    }
}

void ui_view_paint(UiView *self, Gfx *gfx)
{
    gfx_push(gfx);
    gfx_origin(gfx, ui_view_orgin(self));
    gfx_clip(gfx, m_rect(0, 0, self->bound.width, self->bound.height));

    if (self->paint)
    {
        self->paint(self, gfx);
    }

    vec_foreach_v(child, &self->children)
    {
        ui_view_paint(child, gfx);
    }

    gfx_pop(gfx);
}

/* --- Layout --------------------------------------------------------------- */

MRect ui_view_size(MAYBE_UNUSED UiView *self, MAYBE_UNUSED MRect parent)
{
    MRect result = parent;

    UiStyle style = self->style;

    if (self->size)
    {
        result = self->size(self);
        result = ui_spacing_grow(style.padding, style.flow, result);
    }

    result.size = ui_size_apply(style.size, result.size);
    result = ui_spacing_grow(style.margin, self->style.flow, result);

    return result;
}

void ui_view_place(UiView *self, MRect container)
{
    UiStyle style = self->style;

    if (style.placement != M_GRAVITY_NONE)
    {
        MRect bound = ui_view_size(self, container);
        container = m_gravity_apply(style.placement, M_FLOW_LEFT_TO_RIGHT, bound, container);
    }

    container = ui_spacing_shrink(style.margin, self->style.flow, container);
    ui_view_resize(self, container);
}

void ui_layout_dock(UiStyle style, MRect container, UiView *views[], size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        MRect content = ui_view_size(child, container);
        content = ui_dock_apply(child->style.dock, style.flow, content, &container);
        ui_view_place(child, content);
    }
}

void ui_layout_flex(UiStyle style, MRect container, UiView *views[], size_t len)
{
    MFlow flow = style.flow;

    float grows = 0;
    float total = 0;

    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        UiStyle child_style = child->style;

        if (child_style.grow > 0.01)
        {
            grows += child_style.grow;
        }
        else
        {
            total += m_flow_get_width(flow, ui_view_size(child, container));
        }
    }

    float all = m_flow_get_width(flow, container) - style.gaps.x * len;
    float grow_total = m_max(0, all - total);
    float grow_unit = (grow_total) / m_max(1, grows);
    float start = m_flow_get_start(flow, container);

    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        UiStyle child_style = child->style;
        MRect child_bound = {};

        if (child_style.grow > 0.01)
        {
            child_bound = m_flow_set_start(flow, child_bound, start);
            child_bound = m_flow_set_width(flow, child_bound, grow_unit * child_style.grow);
        }
        else
        {
            child_bound = ui_view_size(child, container);
            child_bound = m_flow_set_x(flow, child_bound, start);
        }

        child_bound = m_flow_set_y(flow, child_bound, m_flow_get_top(flow, container));
        child_bound = m_flow_set_height(flow, child_bound, m_flow_get_height(flow, container));

        ui_view_place(child, child_bound);
        start += m_flow_get_width(flow, child_bound) + style.gaps.x;
    }
}

void ui_view_layout(UiView *self)
{
    if (self->layout)
    {
        self->layout(self);
        return;
    }

    UiStyle const style = self->style;

    switch (style.layout)
    {
    default:
    case UI_LAYOUT_DOCK:
        ui_layout_dock(style, ui_view_content(self), self->children.data, self->children.len);
        break;

    case UI_LAYOUT_FLEX:
        ui_layout_flex(style, ui_view_content(self), self->children.data, self->children.len);
        break;

    case UI_LAYOUT_GRID:
        break;
    }

    vec_foreach_v(child, &self->children)
    {
        ui_view_layout(child);
    }
}

/* --- Events --------------------------------------------------------------- */

UiView *ui_view_lookup(UiView *self, MVec2 pos)
{
    if (self->flags & UI_VIEW_GREEDY)
    {
        return self;
    }

    vec_foreach_v(child, &self->children)
    {
        if (m_rect_collide_point(ui_view_bound(child), pos))
        {
            pos = m_vec2_sub(pos, ui_view_orgin(child));
            return ui_view_lookup(child, pos);
        }
    }

    return self;
}

void ui_view_dispatch(UiView *self, UiEvent *event)
{
    if (self->event)
    {
        self->event(self, event);
    }

    if (!event->captured && self->parent)
    {
        ui_view_dispatch(self->parent, event);
    }
}
