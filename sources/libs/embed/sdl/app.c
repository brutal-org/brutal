#include <SDL.h>
#include <embed/app.h>
#include <embed/win.h>
#include <brutal-debug>

void embed_app_init(MAYBE_UNUSED UiApp *self)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
}

void embed_app_deinit(MAYBE_UNUSED UiApp *self)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Quit();
}

static MVec2f _last_mouse_pos = {};

UiEvent sdl_event_to_ui_event(SDL_Event e, uint32_t *handle)
{
    UiEvent result = {};

    switch (e.type)
    {
    case SDL_WINDOWEVENT:
        *handle = e.window.windowID;
        break;

    case SDL_KEYDOWN:
        *handle = e.key.windowID;
        result.type = UI_EVENT_KEYBOARD_DOWN;
        break;

    case SDL_KEYUP:
        *handle = e.key.windowID;
        result.type = UI_EVENT_KEYBOARD_UP;
        break;

    case SDL_MOUSEMOTION:
        if (e.motion.which == SDL_TOUCH_MOUSEID)
        {
            return result;
        }

        *handle = e.motion.windowID;
        result.type = UI_EVENT_MOUSE_MOVE;

        result.mouse.offset.x = e.motion.xrel;
        result.mouse.offset.y = e.motion.yrel;
        result.mouse.position.x = e.motion.x;
        result.mouse.position.y = e.motion.y;

        _last_mouse_pos = result.mouse.position;

        result.mouse.buttons |= (e.motion.state & SDL_BUTTON_LMASK) ? MSBTN_LEFT : 0;
        result.mouse.buttons |= (e.motion.state & SDL_BUTTON_MMASK) ? MSBTN_MIDDLE : 0;
        result.mouse.buttons |= (e.motion.state & SDL_BUTTON_RMASK) ? MSBTN_RIGHT : 0;
        break;

    case SDL_MOUSEBUTTONUP:
        if (e.motion.which == SDL_TOUCH_MOUSEID)
        {
            return result;
        }

        result.type = UI_EVENT_MOUSE_UP;
        *handle = e.button.windowID;
        result.mouse.position = _last_mouse_pos;

        result.mouse.buttons = (e.button.which == SDL_BUTTON_LEFT) ? MSBTN_LEFT : 0;
        result.mouse.buttons = (e.button.which == SDL_BUTTON_RIGHT) ? MSBTN_MIDDLE : 0;
        result.mouse.buttons = (e.button.which == SDL_BUTTON_MIDDLE) ? MSBTN_RIGHT : 0;
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (e.motion.which == SDL_TOUCH_MOUSEID)
        {
            return result;
        }

        result.type = UI_EVENT_MOUSE_DOWN;
        *handle = e.button.windowID;
        result.mouse.position = _last_mouse_pos;

        result.mouse.buttons = (e.button.which == SDL_BUTTON_LEFT) ? MSBTN_LEFT : 0;
        result.mouse.buttons = (e.button.which == SDL_BUTTON_RIGHT) ? MSBTN_MIDDLE : 0;
        result.mouse.buttons = (e.button.which == SDL_BUTTON_MIDDLE) ? MSBTN_RIGHT : 0;
        break;

    case SDL_MOUSEWHEEL:
        if (e.wheel.which == SDL_TOUCH_MOUSEID)
        {
            return result;
        }

        *handle = e.wheel.windowID;
        result.type = UI_EVENT_MOUSE_SCROLL;
        result.mouse.scroll.x = e.wheel.x;
        result.mouse.scroll.y = e.wheel.y;
        result.mouse.position = _last_mouse_pos;
        break;

    default:
        break;
    }

    return result;
}

void embed_app_pump(UiApp *self)
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0 && self->alive)
    {
        uint32_t handle = 0;
        UiEvent event = sdl_event_to_ui_event(e, &handle);

        switch (e.type)
        {
        case SDL_QUIT:
            ui_app_exit(self, 0);
            break;

        case SDL_WINDOWEVENT:
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
            vec_foreach_v(win, &self->windows)
            {
                if (SDL_GetWindowID(win->embed.sdl_window) == handle)
                {
                    ui_win_dispatch(win, &event);
                }
            }
            break;

        default:
            log$("Unhandled SDL event {}", e.type);
            break;
        }
    }
}

void embed_app_wait(MAYBE_UNUSED UiApp *self)
{
    SDL_WaitEventTimeout(nullptr, 1000);
}
