#include <brutal/alloc/global.h>
#include <brutal/time/query.h>
#include <brutal/ui/app.h>
#include <brutal/ui/defer.h>
#include <brutal/ui/win.h>
#include <embed/app.h>

static UiApp *_instance = nullptr;

UiApp *ui_app_self(void)
{
    assert_not_null(_instance);
    return _instance;
}

void ui_app_init(UiApp *self)
{
    vec_init(&self->windows, alloc_global());
    vec_init(&self->defers, alloc_global());

    embed_app_init(self);

    self->alive = true;
    self->font = gfx_font_builtin();
    ui_palette_init(&self->palette);
    _instance = self;
}

void ui_app_deinit(UiApp *self)
{
    embed_app_deinit(self);

    vec_deinit(&self->defers);
    vec_deinit(&self->windows);
}

int ui_app_run(UiApp *self)
{
    while (self->alive)
    {
        ui_app_update(self);
        embed_app_wait(self, ui_app_deadline(self));
        ui_app_pump(self);
    }

    return self->result;
}

void ui_app_pump(UiApp *self)
{
    embed_app_pump(self);
}

void ui_app_attach_win(UiApp *self, UiWin *win)
{
    vec_push(&self->windows, win);
}

void ui_app_detach_win(UiApp *self, UiWin *win)
{
    vec_remove(&self->windows, win);
}

void ui_app_attach_defer(UiApp *self, UiDefer *defer)
{
    vec_push(&self->defers, defer);
}

void ui_app_detach_defer(UiApp *self, UiDefer *defer)
{
    vec_remove(&self->defers, defer);
}

Tick ui_app_deadline(UiApp *self)
{
    Tick deadline = -1;
    vec_foreach_v(defer, &self->defers)
    {
        deadline = m_min(deadline, defer->deadline);
    }
    return deadline;
}

void ui_app_update(UiApp *self)
{
    Tick tick = time_now_ms();

    vec_foreach_v(defer, &self->defers)
    {
        if (defer->deadline <= tick)
        {
            ui_defer_invoke(defer);
        }
    }
}

void ui_app_exit(UiApp *self, int result)
{
    self->result = result;
    self->alive = false;
}

void ui_app_font(UiApp *self, GfxFont font)
{
    self->font = font;
}

GfxColor ui_app_color(UiApp *self, UiRole role)
{
    GfxColor color = GFX_MAGENTA;
    ui_palette_color(&self->palette, role, &color);
    return color;
}

void ui_app_overide_color(UiApp *self, UiRole role, GfxColor color)
{
    ui_palette_overide(&self->palette, role, color);
}
