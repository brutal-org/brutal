#include <SDL.h>
#include <embed/win.h>

void embed_win_init(UiWin *self, MRect bound)
{
    self->embed.sdl_window = SDL_CreateWindow(
        "BRUTAL",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        bound.width,
        bound.height,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI);
}

void embed_win_deinit(UiWin *self)
{
    SDL_DestroyWindow(self->embed.sdl_window);
}

void embed_win_show(UiWin *self)
{
    SDL_ShowWindow(self->embed.sdl_window);
}

void embed_win_hide(UiWin *self)
{
    SDL_HideWindow(self->embed.sdl_window);
}

bool embed_win_visible(UiWin *self)
{
    return SDL_GetWindowFlags(self->embed.sdl_window) & SDL_WINDOW_SHOWN;
}

void embed_win_flip(UiWin *self, MRect rect)
{
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.width, rect.height};
    SDL_UpdateWindowSurfaceRects(self->embed.sdl_window, &sdl_rect, 1);
}

GfxBuf embed_win_gfx(UiWin *self)
{
    SDL_Surface *sdl_surface = SDL_GetWindowSurface(self->embed.sdl_window);
    GfxBuf buf;
    gfx_buf_init(&buf, sdl_surface->w, sdl_surface->h, GFX_FMT_BGRA8888, sdl_surface->pitch, sdl_surface->pixels);
    return buf;
}
