#pragma once

#include <brutal/input/keyboard.h>
#include <brutal/input/mouse.h>
#include <brutal/math/rect.h>

typedef enum
{
    UI_EVENT_IGNORE,

    UI_EVENT_ENTER,
    UI_EVENT_LEAVE,
    UI_EVENT_FOCUS_IN,
    UI_EVENT_FOCUS_OUT,

    UI_EVENT_KEYBOARD_UP,
    UI_EVENT_KEYBOARD_TYPED,
    UI_EVENT_KEYBOARD_DOWN,

    UI_EVENT_MOUSE_UP,
    UI_EVENT_MOUSE_DOWN,
    UI_EVENT_MOUSE_MOVE,
    UI_EVENT_MOUSE_SCROLL,
} UiEventType;

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
    UiEventType type;
    bool captured;

    union
    {
        UiKeyboardEvent keyboard;
        UiMouseEvent mouse;
    };
} UiEvent;
