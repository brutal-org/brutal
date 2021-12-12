#include <brutal/ui/app.h>
#include <embed/app.h>

void ui_app_init(UiApp *self)
{
    embed_app_init(self);
    self->running = true;
}

void ui_app_deinit(UiApp *self)
{
    embed_app_deinit(self);
}

int ui_app_run(UiApp *self)
{
    while (self->running)
    {
        ui_app_pump(self);
    }

    ui_app_deinit(self);

    return self->exit_value;
}

void ui_app_pump(UiApp *self)
{
    embed_app_pump(self);
}

void ui_app_exit(UiApp *self, int exit)
{
    self->exit_value = exit;
    self->running = false;
}
