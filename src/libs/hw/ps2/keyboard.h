#pragma once

#include <brutal-input>
#include <hw/ps2/controller.h>

typedef void (*Ps2KeyboardCallback)(InputEvent ev, void *context);

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
