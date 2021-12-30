#pragma once

#include <brutal/input/keyboard.h>
#include <brutal/input/mouse.h>

typedef enum
{
    UI_EVENT_KEYBOARD,
    UI_EVENT_MOUSE,
} UiEventType;

typedef struct
{
    UiEventType type;
    bool captured;

    union
    {
        KeyboardEvent keyboard;
        MouseEvent mouse;
    };
} UiEvent;
