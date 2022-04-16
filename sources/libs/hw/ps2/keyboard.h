#pragma once

#include <hw/ps2/controller.h>
#include <brutal-ui>

typedef void (*Ps2KeyboardCallback)(UiEvent ev, void *context);

typedef struct
{
    bool kb_escaped;
    BrEvent interrupt_handle;
    Ps2KeyboardCallback callback;
    void *ctx;
    Ps2Controller *controller;
} Ps2Keyboard;

#define PS2_KEYBOARD_ESCAPED 0xE0

void ps2_keyboard_init(Ps2Keyboard *self, Ps2Controller *controller, Ps2KeyboardCallback callback, void *ctx);
