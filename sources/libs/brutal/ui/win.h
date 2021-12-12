#pragma once

#include <brutal/gfx/surface.h>
#include <brutal/ui/app.h>
#include <embed/win-decl.h>

typedef struct
{
    EmbedWin embed;
} UiWin;

void ui_win_init(UiWin *self, int width, int height);

void ui_win_deinit(UiWin *self);

void ui_win_visible(UiWin *self, bool visible);

void ui_win_flip(UiWin *self, Recti rect);

GfxSurface ui_win_gfx(UiWin *self);
