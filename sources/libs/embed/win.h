#pragma once

#include <brutal-ui>

void embed_win_init(UiWin *self, MRectf bound);

void embed_win_deinit(UiWin *self);

void embed_win_show(UiWin *self);

void embed_win_hide(UiWin *self);

bool embed_win_visible(UiWin *self);

void embed_win_flip(UiWin *self, MRectf rect);

GfxBuf embed_win_gfx(UiWin *self);
