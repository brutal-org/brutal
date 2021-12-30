#include <brutal/alloc/global.h>
#include <brutal/debug/assert.h>
#include <brutal/ui/app.h>
#include <brutal/ui/view.h>
#include <brutal/ui/win.h>
#include <embed/win.h>

/* --- Lifecycle ------------------------------------------------------------ */

UiWin *ui_win_create(UiApp *app, MRect bound, int flags)
{
    UiWin *self = alloc_make(alloc_global(), UiWin);
    self->flags = flags;
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

MRect ui_win_bound(UiWin *self)
{
    return m_rect(0, 0, embed_win_gfx(self).width, embed_win_gfx(self).height);
}

MRect ui_win_content(UiWin *self)
{
    MRect rect = ui_win_bound(self);
    rect.pos = (MVec2){};
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
}

void ui_win_should_repaint_rect(MAYBE_UNUSED UiWin *self, MAYBE_UNUSED MRect rect)
{
}

void ui_win_repaint(UiWin *self)
{
    gfx_begin(&self->gfx, ui_win_gfx(self));
    gfx_clear(&self->gfx, GFX_BLACK);

    if (self->paint)
    {
        self->paint(self, &self->gfx);
    }

    if (self->root)
    {
        ui_view_paint(self->root, &self->gfx);
    }

    gfx_end(&self->gfx);

    ui_win_flip_full(self);
}

void ui_win_flip(UiWin *self, MRect rect)
{
    return embed_win_flip(self, rect);
}

void ui_win_flip_full(UiWin *self)
{
    ui_win_flip(self, ui_win_bound(self));
}

/* --- Layout --------------------------------------------------------------- */

void ui_win_relayout(UiWin *self)
{
    if (!self->root)
    {
        return;
    }

    ui_view_resize(self->root, ui_win_content(self));
    ui_view_layout(self->root);
}

/* --- Events --------------------------------------------------------------- */

void ui_win_dispatch(UiWin *self, UiEvent *event)
{
    if (self->event)
    {
        self->event(self, event);
    }

    if (self->root)
    {
        ui_view_event(self->root, event);
    }
}
