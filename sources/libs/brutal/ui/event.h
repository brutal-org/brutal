#pragma once

#include <brutal/input/keyboard.h>
#include <brutal/input/mouse.h>
#include <brutal/math/rect.h>

typedef int UiWinHandle;

typedef enum
{
    UI_EVENT_IGNORE,

    UI_EVENT_WINDOW_GET_FOCUS,
    UI_EVENT_WINDOW_LOST_FOCUS,
    UI_EVENT_WINDOW_MOVE,
    UI_EVENT_WINDOW_RESIZE,

    UI_EVENT_KEYBOARD_UP,
    UI_EVENT_KEYBOARD_DOWN,

    UI_EVENT_MOUSE_UP,
    UI_EVENT_MOUSE_DOWN,
    UI_EVENT_MOUSE_MOVE,
    UI_EVENT_MOUSE_SCROLL,
    UI_EVENT_MOUSE_ENTER,
    UI_EVENT_MOUSE_LEAVE,
} UiEventType;

typedef struct
{
    MRect bound;
} UiWindowEvent;

typedef struct
{
    KbKey key;
    KbMod modifiers;
    KbMotion motion;
    Rune rune;
} UiKeyboardEvent;

typedef struct
{
    MVec2 position;
    MVec2 offset;
    MVec2 scroll;

    MsBtns buttons;
} UiMouseEvent;

typedef struct
{
    UiWinHandle handle;
    UiEventType type;
    bool captured;

    union
    {
        UiWindowEvent window;
        UiKeyboardEvent keyboard;
        UiMouseEvent mouse;
    };
} UiEvent;
