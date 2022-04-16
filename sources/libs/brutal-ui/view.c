#include "view.h"

#include "app.h"
#include "parser.h"
#include "win.h"

/* --- Lifecycle ------------------------------------------------------------ */

void ui_view_init(UiView *self)
{
    vec_init(&self->children, alloc_global());
    self->bound = m_rectf(0, 0, 100, 100);
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

MVec2f ui_view_orgin(UiView *self)
{
    return m_vec2f_add(self->scroll, self->bound.pos);
}

MRectf ui_view_bound(UiView *self)
{
    return self->bound;
}

MRectf ui_view_content(UiView *self)
{
    return m_spacing_shrink(self->layout.padding, self->layout.flow, ui_view_container(self));
}

MRectf ui_view_container(UiView *self)
{
    MRectf rect = self->bound;
    rect.pos = m_vec2f(0, 0);
    return rect;
}

void ui_view_resize(UiView *self, MRectf rect)
{
    self->bound = rect;
}

void ui_view_layout(UiView *self, char const *layout)
{
    ui_parse_layout_str(&self->layout, str$(layout));
    ui_view_should_relayout(self);
}

GfxColor ui_view_color(UiView *self, UiRole role)
{
    GfxColor color;
    if (!ui_palette_color(&self->palette, role, &color))
    {
        if (self->parent)
        {
            color = ui_view_color(self->parent, role);
        }
        else
        {
            color = ui_app_color(ui_app_self(), role);
        }
    }
    return color;
}

void ui_view_overide_color(UiView *self, UiRole role, GfxColor color)
{
    ui_palette_overide(&self->palette, role, color);
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

void ui_view_should_repaint_rect(UiView *self, MRectf dirty)
{
    if (!m_rectf_collide_rect(self->bound, dirty))
    {
        return;
    }

    if (self->parent)
    {
        dirty.pos = m_vec2f_add(dirty.pos, ui_view_orgin(self->parent));
        ui_view_should_repaint_rect(self->parent, dirty);
    }
    else if (self->window)
    {
        ui_win_should_repaint_rect(self->window, dirty);
    }
}

void ui_view_repaint(UiView *self, Gfx *gfx)
{
    gfx_push(gfx);
    gfx_origin(gfx, ui_view_orgin(self));
    gfx_clip(gfx, m_rectf(0, 0, self->bound.width, self->bound.height));

    if (self->repaint)
    {
        self->repaint(self, gfx);
    }

    vec_foreach_v(child, &self->children)
    {
        ui_view_repaint(child, gfx);
    }

    // gfx_fill_style(gfx, gfx_paint_fill(GFX_CYAN));
    // gfx_stroke_rect(gfx, ui_view_container(self), 1);

    gfx_pop(gfx);
}

/* --- Layout --------------------------------------------------------------- */

MRectf ui_view_size(MAYBE_UNUSED UiView *self)
{
    MRectf result;

    UiLayout layout = self->layout;

    if (self->size)
    {
        result = self->size(self);
    }
    else
    {
        result.size = ui_layout_size(&layout, self->children.data, self->children.len);
    }

    result = m_spacing_grow(layout.padding, layout.flow, result);
    result.size = ui_size_apply(layout.size, result.size);
    result = m_spacing_grow(layout.margin, layout.flow, result);

    return result;
}

void ui_view_place(UiView *self, MRectf container)
{
    UiLayout layout = self->layout;

    MRectf bound = ui_view_size(self);
    container = m_gravity_apply(layout.placement, M_FLOW_LEFT_TO_RIGHT, bound, container);
    container = m_spacing_shrink(layout.margin, layout.flow, container);

    ui_view_resize(self, container);
}

void ui_view_relayout(UiView *self)
{
    if (vec_len(&self->children) == 0)
    {
        return;
    }

    if (self->relayout)
    {
        self->relayout(self);
    }
    else
    {
        ui_layout_run(
            &self->layout,
            ui_view_content(self),
            self->children.data,
            self->children.len);
    }

    vec_foreach_v(child, &self->children)
    {
        ui_view_relayout(child);
    }
}

void ui_view_should_relayout(UiView *self)
{
    if (self->parent)
    {
        ui_view_relayout(self->parent);
    }

    if (self->window)
    {
        ui_win_should_relayout(self->window);
    }
}

/* --- Events --------------------------------------------------------------- */

UiView *ui_view_lookup(UiView *self, MVec2f pos)
{
    if (self->flags & UI_VIEW_GREEDY)
    {
        return self;
    }

    vec_foreach_v(child, &self->children)
    {
        if (m_rectf_collide_point(ui_view_bound(child), pos))
        {
            pos = m_vec2f_sub(pos, ui_view_orgin(child));
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
