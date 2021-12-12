#pragma once

#include <brutal/base/std.h>
#include <embed/app-decl.h>

typedef struct
{
    EmbedApp embed;

    bool running;
    int exit_value;
} UiApp;

void ui_app_init(UiApp *self);

void ui_app_deinit(UiApp *self);

int ui_app_run(UiApp *self);

void ui_app_pump(UiApp *self);

void ui_app_exit(UiApp *self, int exit);
