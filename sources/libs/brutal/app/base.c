#include <brutal/app/base.h>

void app_init(App *self)
{
    embed_app_init(&self->host);
    self->running = true;
}

void app_deinit(App *self)
{
    embed_app_deinit(&self->host);
}

int app_run(App *self)
{
    while (self->running)
    {
        app_pump(self);
    }

    app_deinit(self);

    return self->exit_value;
}

void app_pump(App *self)
{
    embed_app_pump(&self->host);
}

void app_exit(App *self, int exit)
{
    self->exit_value = exit;
    self->running = false;
}
