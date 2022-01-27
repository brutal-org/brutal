#pragma once

#include <brutal/ds/vec.h>
#include <brutal/ui/event.h>
#include <embed/app-decl.h>

typedef struct _UiWin UiWin;

typedef struct _UiApp
{
    EmbedApp embed;
    bool alive;
    int result;

    Vec(UiWin *) windows;
} UiApp;

void ui_app_init(UiApp *self);

void ui_app_deinit(UiApp *self);

int ui_app_run(UiApp *self);

void ui_app_pump(UiApp *self);

void ui_app_attach_win(UiApp *self, UiWin *win);

void ui_app_detach_win(UiApp *self, UiWin *win);

void ui_app_exit(UiApp *self, int result);
