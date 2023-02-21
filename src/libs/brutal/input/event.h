#pragma once

#include <brutal/math/vec2.h>
#include <brutal/text/rune.h>

#include "keyboard.h"
#include "mouse.h"

enum
{
    INPUT_EVENT_IGNORE,

    INPUT_EVENT_ENTER,
    INPUT_EVENT_LEAVE,
    INPUT_EVENT_FOCUS_IN,
    INPUT_EVENT_FOCUS_OUT,

    INPUT_EVENT_KEYBOARD_UP,
    INPUT_EVENT_KEYBOARD_TYPED,
    INPUT_EVENT_KEYBOARD_DOWN,

    INPUT_EVENT_MOUSE_UP,
    INPUT_EVENT_MOUSE_DOWN,
    INPUT_EVENT_MOUSE_MOVE,
    INPUT_EVENT_MOUSE_SCROLL,
};

typedef uint8_t InputEventType;

typedef struct
{
    KbKey key;
    KbMod modifiers;
    Rune rune;
} InputKeyboardEvent;

typedef struct
{
    MVec2f position;
    MVec2f offset;
    MVec2f scroll;
    MsBtns buttons;
} InputMouseEvent;

typedef struct
{
    InputEventType type;
    bool captured;

    union
    {
        InputKeyboardEvent keyboard;
        InputMouseEvent mouse;
    };
} InputEvent;

static inline bool input_event_is_keyboard(InputEvent *event)
{
    return event->type >= INPUT_EVENT_KEYBOARD_UP && event->type <= INPUT_EVENT_KEYBOARD_DOWN;
}

static inline bool input_event_is_mouse(InputEvent *event)
{
    return event->type >= INPUT_EVENT_MOUSE_UP && event->type <= INPUT_EVENT_MOUSE_SCROLL;
}
