#pragma once 

#include <hw/ps2/controller.h>
#include <hw/ps2/mouse.h>
#include <hw/ps2/keyboard.h>


typedef struct 
{
    Ps2Controller controller; 
    Ps2Mouse mouse;
    Ps2Keyboard keyboard;
} Ps2;

typedef enum
{
    PS2_NONE = 0,
    PS2_MOUSE = 1 << 1,
    PS2_KEYBOARD = 1 << 2,
} Ps2Devices;

void init_ps2_mouse(Ps2* ps2, Ps2MouseCallback callback, void* ctx);

void init_ps2_keyboard(Ps2* ps2, Ps2KeyboardCallback callback, void* ctx);

Ps2Devices ps2_interrupt_handle(Ps2* ps2, BrEvent ev);
