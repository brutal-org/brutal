#pragma once

#include <brutal-input>
#include <brutal-math>

enum
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
};

typedef uint8_t UiEventType;

typedef struct
{
    KbKey key;
    KbMod modifiers;
    Rune rune;
} UiKeyboardEvent;

typedef struct
{
    MVec2f position;
    MVec2f offset;
    MVec2f scroll;
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

static inline bool ui_event_is_keyboard(UiEvent *event)
{
    return event->type >= UI_EVENT_KEYBOARD_UP && event->type <= UI_EVENT_KEYBOARD_DOWN;
}

static inline bool ui_event_is_mouse(UiEvent *event)
{
    return event->type >= UI_EVENT_MOUSE_UP && event->type <= UI_EVENT_MOUSE_SCROLL;
}
