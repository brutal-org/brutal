#include <brutal/alloc/global.h>
#include <brutal/ui/app.h>
#include <brutal/ui/win.h>
#include <embed/app.h>

static UiApp *_instance;

UiApp *ui_app_self(void)
{
    assert_not_null(_instance);
    return _instance;
}

void ui_app_init(UiApp *self)
{
    vec_init(&self->windows, alloc_global());
    embed_app_init(self);
    self->alive = true;
    self->font = gfx_font_builtin();
    ui_palette_init(&self->palette);
    _instance = self;
}

void ui_app_deinit(UiApp *self)
{
    embed_app_deinit(self);
    vec_deinit(&self->windows);
}

int ui_app_run(UiApp *self)
{
    while (self->alive)
    {
        embed_app_wait(self);
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
