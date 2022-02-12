#include <bal/abi.h>
#include <hw/ps2/ps2.h>

void init_ps2_mouse(Ps2Mouse* mouse, Ps2Controller* controller, Ps2MouseCallback callback, void *ctx)
{
    *mouse = (Ps2Mouse){};
    ps2_second_port_init(controller);
    mouse->callback = callback;
    mouse->ctx = ctx;
    _ps2_mouse_init(mouse, controller);
}

void init_ps2_keyboard(Ps2Keyboard *kb, Ps2Controller* controller,  Ps2KeyboardCallback callback, void *ctx)
{
    *kb = (Ps2Keyboard){};
    ps2_first_port_init(controller);
    kb->callback = callback;
    kb->ctx = ctx;
    _ps2_keyboard_init(kb, controller);
}
