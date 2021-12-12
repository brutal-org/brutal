#include <embed/win.h>

void embed_win_init(UiWin *self, int width, int height)
{
    self->embed.sdl_window = SDL_CreateWindow(
        "QEMU",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI);
}

void embed_win_deinit(UiWin *self)
{
    SDL_DestroyWindow(self->embed.sdl_window);
}

void embed_win_visible(UiWin *self, bool visible)
{
    if (visible)
    {
        SDL_ShowWindow(self->embed.sdl_window);
    }
    else
    {
        SDL_HideWindow(self->embed.sdl_window);
    }
}

void embed_win_flip(UiWin *self, Recti rect)
{
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_UpdateWindowSurfaceRects(self->embed.sdl_window, &sdl_rect, 1);
}

GfxSurface embed_win_gfx(UiWin *self)
{
    SDL_Surface *sdl_surface = SDL_GetWindowSurface(self->embed.sdl_window);

    return (GfxSurface){
        .width = sdl_surface->w,
        .height = sdl_surface->h,
        .pitch = sdl_surface->pitch,
        .format = GFX_PIXEL_FORMAT_BGRA8888,
        .buf = sdl_surface->pixels,
    };
}
