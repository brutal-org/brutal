#pragma once

#include <brutal/ds/vec.h>
#include <brutal/gfx/font.h>
#include <brutal/time/mod.h>
#include <brutal/ui/color.h>
#include <brutal/ui/event.h>
#include <embed/app-decl.h>

typedef struct _UiWin UiWin;
typedef struct _UiDefer UiDefer;

typedef struct _UiApp
{
    EmbedApp embed;
    bool alive;
    int result;
    GfxFont font;
    UiPalette palette;

    Vec(UiWin *) windows;
    Vec(UiDefer *) defers;
} UiApp;

UiApp *ui_app_self(void);

void ui_app_init(UiApp *self);

void ui_app_deinit(UiApp *self);

int ui_app_run(UiApp *self);

void ui_app_pump(UiApp *self);

void ui_app_attach_win(UiApp *self, UiWin *win);

void ui_app_detach_win(UiApp *self, UiWin *win);

void ui_app_attach_defer(UiApp *self, UiDefer *defer);

void ui_app_detach_defer(UiApp *self, UiDefer *defer);

Time ui_app_deadline(UiApp *self);

void ui_app_update(UiApp *self);

void ui_app_exit(UiApp *self, int result);

void ui_app_font(UiApp *self, GfxFont font);

GfxColor ui_app_color(UiApp *self, UiRole role);

void ui_app_overide_color(UiApp *self, UiRole role, GfxColor color);
