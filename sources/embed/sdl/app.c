#include <SDL.h>
#include <brutal/base.h>
#include <embed/app.h>

void host_app_init(MAYBE_UNUSED HostApp *self)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
}

void host_app_deinit(MAYBE_UNUSED HostApp *self)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Quit();
}

void host_app_pump(MAYBE_UNUSED HostApp *self)
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
    }
}
