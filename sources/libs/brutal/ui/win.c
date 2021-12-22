#include <brutal/alloc/global.h>
#include <brutal/debug/assert.h>
#include <brutal/ui/app.h>
#include <brutal/ui/win.h>
#include <embed/win.h>

/* --- Lifecycle ------------------------------------------------------------ */

void ui_win_init(UiWin *self, UiApp *app, Rect bound, int flags)
{
    self->flags = flags;
    self->app = app;

    gfx_init(&self->gfx, alloc_global());
    embed_win_init(self, bound);
    ui_app_attach_win(app, self);
}

void ui_win_destroy(UiWin *self)
{
    ui_app_attach_win(self->app, self);
    embed_win_deinit(self);
    gfx_deinit(&self->gfx);

    if (self->deinit)
    {
        self->deinit(self);
    }
}

/* --- Properties --------------------------------------------------------------- */

void ui_win_hide(UiWin *self)
{
    embed_win_hide(self);
}

void ui_win_show(UiWin *self)
{
    embed_win_show(self);
}

bool ui_win_visible(UiWin *self)
{
    return embed_win_visible(self);
}

Rect ui_win_bound(UiWin *self)
{
    return (Rect){
        0,
        0,
        embed_win_gfx(self).width,
        embed_win_gfx(self).height,
    };
}

/* --- Paint ---------------------------------------------------------------- */

GfxBuf ui_win_gfx(UiWin *self)
{
    return embed_win_gfx(self);
}

void ui_win_repaint(UiWin *self)
{
    gfx_begin(&self->gfx, ui_win_gfx(self));

    if (self->paint)
    {
        self->paint(self, &self->gfx);
    }

    gfx_end(&self->gfx);

    ui_win_flip_full(self);
}

void ui_win_flip(UiWin *self, Rect rect)
{
    return embed_win_flip(self, rect);
}

void ui_win_flip_full(UiWin *self)
{
    ui_win_flip(self, ui_win_bound(self));
}

/* --- Events --------------------------------------------------------------- */

void ui_win_dispatch(UiWin *self, UiEvent *event)
{
    if (self->event)
    {
        self->event(self, event);
    }
}
