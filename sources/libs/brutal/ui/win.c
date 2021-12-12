#include <brutal/ui/win.h>
#include <embed/win.h>

void ui_win_init(UiWin *self, int width, int height)
{
    embed_win_init(self, width, height);
}

void ui_win_deinit(UiWin *self)
{
    embed_win_deinit(self);
}

void ui_win_visible(UiWin *self, bool visible)
{
    embed_win_visible(self, visible);
}

void ui_win_flip(UiWin *self, Recti rect)
{
    return embed_win_flip(self, rect);
}

GfxSurface ui_win_gfx(UiWin *self)
{
    return embed_win_gfx(self);
}
