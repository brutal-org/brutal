#include <hw/ps2/ps2.h>
#include "bal/abi/syscalls.h"
#include "bal/abi/types.h"
#include "ps2/controller.h"
#include "ps2/mouse.h"


void init_ps2_mouse(Ps2* ps2, Ps2MouseCallback callback, void* ctx)
{
    ps2_second_port_init(&ps2->controller);
    ps2->mouse.interrupt_handle = (BrEvent){
        .type = BR_EVENT_IRQ,
        .irq = 12,
    };
    br_bind(&(BrBindArgs){.event = ps2->mouse.interrupt_handle});

    ps2->mouse.callback = callback;
    ps2->mouse.ctx = ctx;
    _ps2_mouse_init(&ps2->mouse, &ps2->controller);

}

void init_ps2_keyboard(Ps2* ps2, Ps2KeyboardCallback callback, void* ctx)
{

    ps2_first_port_init(&ps2->controller);
    ps2->keyboard.interrupt_handle = (BrEvent){
        .type = BR_EVENT_IRQ,
        .irq = 1,
    };

    br_bind(&(BrBindArgs){
        .event = ps2->keyboard.interrupt_handle});

    ps2->keyboard.callback = callback;
    ps2->keyboard.ctx = ctx;
    _ps2_keyboard_init(&ps2->keyboard, &ps2->controller);
}

Ps2Devices ps2_interrupt_handle(Ps2 *ps2, BrEvent ev)
{
    if (ev.irq != 1 && ev.irq != 12)
    {
        return PS2_NONE;
    }

    Ps2Devices devices = PS2_NONE;

    if (ev.irq == 1 && ps2_keyboard_interrupt_handle(&ps2->keyboard, &ps2->controller))
    {
        devices |= PS2_KEYBOARD;
    }
    else if (ev.irq == 12 && ps2_mouse_interrupt_handle(&ps2->mouse, &ps2->controller))
    {
        devices |= PS2_MOUSE;
    }

    if(devices != 0)
    {
        br_ack(&(BrAckArgs){.event = ev});
    }
    return devices;
}
