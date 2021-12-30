#include <SDL.h>
#include <brutal/debug.h>
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
        switch (e.type)
        {
        case SDL_QUIT:
            log$("SDL_QUIT");
            ui_app_exit(self, 0);
            break;

        case SDL_KEYDOWN:
            break;

        case SDL_KEYUP:
            break;
        }
    }
}

void embed_app_wait(MAYBE_UNUSED UiApp *self)
{
    SDL_WaitEventTimeout(nullptr, 1000);
}
