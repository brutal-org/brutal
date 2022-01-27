#include <brutal/alloc/global.h>
#include <brutal/ui/app.h>
#include <brutal/ui/win.h>
#include <embed/app.h>

void ui_app_init(UiApp *self)
{
    vec_init(&self->windows, alloc_global());
    embed_app_init(self);
    self->alive = true;
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
        ui_app_pump(self);
        embed_app_wait(self);
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
