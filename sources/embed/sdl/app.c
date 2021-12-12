#include <SDL.h>
#include <brutal/base.h>
#include <embed/app.h>

void embed_app_init(MAYBE_UNUSED UiApp *self)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
}

void embed_app_deinit(MAYBE_UNUSED UiApp *self)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Quit();
}

void embed_app_pump(MAYBE_UNUSED UiApp *self)
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
    }
}
