#include <brutal-alloc>
#include <brutal-debug>
#include <embed/win.h>

#include "app.h"
#include "view.h"
#include "win.h"

/* --- Lifecycle ------------------------------------------------------------ */

UiWin *ui_win_create(UiApp *app, MRectf bound, UiWinType type)
{
    UiWin *self = alloc_make(alloc_global(), UiWin);

    self->type = type;
    self->app = app;

    gfx_init(&self->gfx, alloc_global());
    embed_win_init(self, bound);
    ui_app_attach_win(app, self);

    return self;
}

void ui_win_ref(UiWin *self)
{
    self->refcount++;
}

void ui_win_deref(UiWin *self)
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

    ui_app_attach_win(self->app, self);
    embed_win_deinit(self);
    gfx_deinit(&self->gfx);

    alloc_free(alloc_global(), self);
}

/* --- Properties --------------------------------------------------------------- */

void ui_win_hide(UiWin *self)
{
    embed_win_hide(self);
}

void ui_win_show(UiWin *self)
{
    ui_win_relayout(self);
    ui_win_repaint(self);
    embed_win_show(self);

    ui_win_flip_full(self);
}

bool ui_win_visible(UiWin *self)
{
    return embed_win_visible(self);
}

MRectf ui_win_bound(UiWin *self)
{
    return m_rectf(0, 0, embed_win_gfx(self).width, embed_win_gfx(self).height);
}

MRectf ui_win_content(UiWin *self)
{
    MRectf rect = ui_win_bound(self);
    rect.pos = (MVec2f){};
    return rect;
}

void ui_win_mount(UiWin *self, UiView *view)
{
    if (self->root)
    {
        ui_view_unmounted(self->root);
        ui_view_deref(self->root);
        self->root = nullptr;
    }

    if (view)
    {
        ui_view_ref(view);
        self->root = view;
        ui_view_mounted_to_window(view, self);
    }
}

/* --- Paint ---------------------------------------------------------------- */

GfxBuf ui_win_gfx(UiWin *self)
{
    return embed_win_gfx(self);
}

void ui_win_should_repaint(MAYBE_UNUSED UiWin *self)
{
    ui_win_should_repaint_rect(self, ui_win_bound(self));
}

void ui_win_should_repaint_rect(UiWin *self, MAYBE_UNUSED MRectf rect)
{
    ui_win_repaint_rect(self, rect);
    ui_win_flip(self, rect);
}

void ui_win_repaint(UiWin *self)
{
    ui_win_repaint_rect(self, ui_win_bound(self));
}

void ui_win_repaint_rect(UiWin *self, MRectf rect)
{
    gfx_begin(&self->gfx, ui_win_gfx(self));
    gfx_clip(&self->gfx, rect);
    gfx_clear(&self->gfx, GFX_BLACK);
    gfx_font_style(&self->gfx, UI_FONT_BODY);
    gfx_font_family(&self->gfx, self->app->font);

    if (self->paint)
    {
        gfx_push(&self->gfx);
        self->paint(self, &self->gfx);
        gfx_pop(&self->gfx);
    }

    if (self->root)
    {
        gfx_push(&self->gfx);
        ui_view_repaint(self->root, &self->gfx);
        gfx_pop(&self->gfx);
    }

    // gfx_fill_style(&self->gfx, gfx_paint_fill(gfx_color_rand(100)));
    // gfx_fill_rect(&self->gfx, rect, 0);

    gfx_end(&self->gfx);
}

void ui_win_flip(UiWin *self, MRectf rect)
{
    return embed_win_flip(self, rect);
}

void ui_win_flip_full(UiWin *self)
{
    ui_win_flip(self, ui_win_bound(self));
}

/* --- Layout --------------------------------------------------------------- */

void ui_win_should_relayout(UiWin *self)
{
    ui_win_relayout(self);
    ui_win_repaint(self);
}

void ui_win_relayout(UiWin *self)
{
    if (!self->root)
    {
        return;
    }

    ui_view_resize(self->root, ui_win_content(self));
    ui_view_relayout(self->root);
}

/* --- Events --------------------------------------------------------------- */

void ui_win_update_overing(UiWin *self, UiEvent *event)
{
    if (!self->root)
        return;

    UiView *overing = ui_view_lookup(self->root, event->mouse.position);

    if (self->overing != overing)
    {
        if (self->overing)
        {
            UiEvent mouse_leave_event = {
                .type = UI_EVENT_LEAVE,
                .mouse = event->mouse,
            };

            ui_view_dispatch(self->overing, &mouse_leave_event);

            ui_view_deref(self->overing);
            self->overing = nullptr;
        }

        if (overing)
        {
            UiEvent mouse_enter_event = {
                .type = UI_EVENT_ENTER,
                .mouse = event->mouse,
            };

            ui_view_dispatch(overing, &mouse_enter_event);

            ui_view_ref(overing);
            self->overing = overing;
        }
    }
}

void ui_win_handle_keyboard_up(UiWin *self, UiEvent *event)
{
    if (self->focus)
    {
        ui_view_dispatch(self->overing, event);
    }
}

void ui_win_handle_keyboard_down(UiWin *self, UiEvent *event)
{
    if (self->focus)
    {
        ui_view_dispatch(self->overing, event);
    }
}

void ui_win_handle_mouse_up(UiWin *self, UiEvent *event)
{
    self->grabbed = false;

    if (self->overing)
    {
        ui_view_dispatch(self->overing, event);
    }

    ui_win_update_overing(self, event);
}

void ui_win_handle_mouse_down(UiWin *self, UiEvent *event)
{
    if (self->focus != self->overing)
    {
        if (self->focus)
        {
            ui_view_deref(self->focus);
            self->focus = nullptr;
        }

        if (self->overing)
        {
            ui_view_ref(self->overing);
            self->focus = self->overing;
        }
    }

    self->grabbed = true;

    if (self->overing)
    {
        ui_view_dispatch(self->overing, event);
    }
}

void ui_win_handle_mouse_move(UiWin *self, UiEvent *event)
{
    if (!self->grabbed)
    {
        ui_win_update_overing(self, event);
    }

    if (self->overing)
    {
        ui_view_dispatch(self->overing, event);
    }
}

void ui_win_dispatch(UiWin *self, UiEvent *event)
{
    switch (event->type)
    {
    case UI_EVENT_KEYBOARD_UP:
        ui_win_handle_keyboard_up(self, event);
        break;

    case UI_EVENT_KEYBOARD_DOWN:
        ui_win_handle_keyboard_down(self, event);
        break;

    case UI_EVENT_MOUSE_UP:
        ui_win_handle_mouse_up(self, event);
        break;

    case UI_EVENT_MOUSE_DOWN:
        ui_win_handle_mouse_down(self, event);
        break;

    case UI_EVENT_MOUSE_MOVE:
        ui_win_handle_mouse_move(self, event);
        break;

    default:
        break;
    }

    if (!event->captured && self->event)
    {
        self->event(self, event);
    }
}
