#pragma once

#include <brutal/ds.h>
#include <embed/app-decl.h>

typedef struct ui_win UiWin;

typedef struct ui_app
{
    bool alive;
    int result;

    Vec(UiWin *) windows;

    EmbedApp embed;
} UiApp;

void ui_app_init(UiApp *self);

void ui_app_deinit(UiApp *self);

int ui_app_run(UiApp *self);

void ui_app_pump(UiApp *self);

void ui_app_animate(UiApp *self);

void ui_app_attach_win(UiApp *self, UiWin *win);

void ui_app_detach_win(UiApp *self, UiWin *win);

void ui_app_exit(UiApp *self, int result);
