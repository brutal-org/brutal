#pragma once

#include <hw/ps2/controller.h>
#include <hw/ps2/keyboard.h>
#include <hw/ps2/mouse.h>
#include <ipc/component.h>


void init_ps2_mouse(Ps2Mouse*mouse, Ps2Controller* controller,  Ps2KeyboardCallback callback, void *ctx);

void init_ps2_keyboard(Ps2Keyboard *kb, Ps2Controller* controller,  Ps2KeyboardCallback callback, void *ctx);
