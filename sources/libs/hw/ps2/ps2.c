#include <bal/abi.h>
#include <hw/ps2/ps2.h>
#include "ps2/controller.h"
#include "ps2/mouse.h"

void ps2_interrupt_handle(Ps2 *ps2, BrEvent ev)
{
    if (ev.type != BR_EVENT_IRQ || (ev.irq != 1 && ev.irq != 12))
    {
        return;
    }
    if (ev.irq == 1 && ps2_keyboard_interrupt_handle(&ps2->keyboard, &ps2->controller))
    {
        br_ack(&(BrAckArgs){.event = ev});
        return;
    }
    else if (ev.irq == 12 && ps2_mouse_interrupt_handle(&ps2->mouse, &ps2->controller))
    {
        br_ack(&(BrAckArgs){.event = ev});
        return;
    }
    br_ack(&(BrAckArgs){.event = ev});
}

void init_ps2_mouse(Ps2 *ps2, Ps2MouseCallback callback, void *ctx)
{
    ps2_second_port_init(&ps2->controller);
    ps2->mouse.interrupt_handle = (BrEvent){
        .type = BR_EVENT_IRQ,
        .irq = 12,
    };
    ipc_component_bind(ps2->ipc, ps2->mouse.interrupt_handle, (IpcEventHandler *)ps2_interrupt_handle, ps2);
    ps2->mouse.callback = callback;
    ps2->mouse.ctx = ctx;
    _ps2_mouse_init(&ps2->mouse, &ps2->controller);
}

void init_ps2_keyboard(Ps2 *ps2, Ps2KeyboardCallback callback, void *ctx)
{

    ps2_first_port_init(&ps2->controller);
    ps2->keyboard.interrupt_handle = (BrEvent){
        .type = BR_EVENT_IRQ,
        .irq = 1,
    };

    ipc_component_bind(ps2->ipc, ps2->keyboard.interrupt_handle, (IpcEventHandler *)ps2_interrupt_handle, ps2);

    ps2->keyboard.callback = callback;
    ps2->keyboard.ctx = ctx;
    _ps2_keyboard_init(&ps2->keyboard, &ps2->controller);
}
void init_ps2(Ps2 *ps2, IpcComponent *ipc)
{
    ps2->ipc = ipc;
    log$("owo");
    ps2_controller_init(&ps2->controller);
    log$("awa");
}
