#pragma once

#include <hw/ps2/controller.h>
#include "brutal/ui/event.h"

typedef void (*Ps2KeyboardCallback)(UiKeyboardEvent ev, void *context);

typedef struct
{
    bool kb_escaped;
    BrEvent interrupt_handle;
    Ps2KeyboardCallback callback;
    void *ctx;
} Ps2Keyboard;

#define PS2_KEYBOARD_ESCAPED 0xE0

void _ps2_keyboard_init(Ps2Keyboard *self, Ps2Controller *controller);

bool ps2_keyboard_interrupt_handle(Ps2Keyboard *self, Ps2Controller *controller);
