#include <embed/ui.h>

void embed_surface_init(EmbedUiSurface *self, int width, int height)
{
    self->sdl_window = SDL_CreateWindow(
        "QEMU",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI);
}

void embed_surface_deinit(EmbedUiSurface *self)
{
    SDL_DestroyWindow(self->sdl_window);
}

void embed_surface_visible(EmbedUiSurface *self, bool visible)
{
    if (visible)
    {
        SDL_ShowWindow(self->sdl_window);
    }
    else
    {
        SDL_HideWindow(self->sdl_window);
    }
}

void embed_surface_flip(EmbedUiSurface *self, Recti rect)
{
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_UpdateWindowSurfaceRects(self->sdl_window, &sdl_rect, 1);
}

GfxSurface embed_surface_surface(EmbedUiSurface *self)
{
    SDL_Surface *sdl_surface = SDL_GetWindowSurface(self->sdl_window);

    return (GfxSurface){
        .width = sdl_surface->w,
        .height = sdl_surface->h,
        .pitch = sdl_surface->pitch,
        .format = GFX_PIXEL_FORMAT_BGRA8888,
        .buf = sdl_surface->pixels,
    };
}
