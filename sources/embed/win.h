#pragma once

#include <brutal/gfx.h>
#include <brutal/ui.h>

void embed_win_init(UiWin *self, int width, int height);

void embed_win_deinit(UiWin *self);

void embed_win_visible(UiWin *self, bool visible);

void embed_win_flip(UiWin *self, Recti rect);

GfxSurface embed_win_gfx(UiWin *self);
