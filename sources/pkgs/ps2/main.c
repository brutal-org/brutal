#include <bal/abi.h>
#include <bal/hw.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/ui.h>
#include <ipc/ipc.h>
#include <protos/event.h>
#include "bal/abi/types.h"
#include "brutal/ui/event.h"
#include <hw/ps2/ps2.h>

typedef struct
{
    BalIo io;

    IpcCap input_sink;
} Ps2Ctx;

void ps2_keyboard_callback(UiKeyboardEvent ev, void* ps2)
{
    bool handled = false;

     UiEvent event = {
        .type = UI_EVENT_KEYBOARD_TYPED,
        .keyboard = ev,
    };


    event_sink_dispatch_rpc(ipc_component_self(), ((Ps2Ctx*)ps2)->input_sink, &event, &handled, alloc_global());
}

void ps2_mouse_callback(UiMouseEvent ev, void* ps2)
{
    bool handled = false;

     UiEvent event = {
        .type = UI_EVENT_MOUSE_MOVE,
        .mouse = ev,
    };


    event_sink_dispatch_rpc(ipc_component_self(), ((Ps2Ctx*)ps2)->input_sink, &event, &handled, alloc_global());
}


int ipc_component_main(IpcComponent *self)
{
    Ps2 ps2 = {};
    Ps2Ctx ctx = {};
    init_ps2_mouse(&ps2, &ps2_mouse_callback, &ctx);
    init_ps2_keyboard(&ps2, &ps2_keyboard_callback, &ctx);

    ctx.input_sink = ipc_component_require(self, IPC_EVENT_SINK_PROTO);

    BrIpcArgs ipc;
    ipc.flags = BR_IPC_RECV | BR_IPC_BLOCK;
    ipc.deadline = BR_DEADLINE_INFINITY;


    while (br_ipc(&ipc) == BR_SUCCESS)
    {
        BrMsg msg = ipc.msg;

        if (br_addr_eq(msg.from, BR_ADDR_EVENT) && msg.event.type == BR_EVENT_IRQ)
        {

            ps2_interrupt_handle(&ps2, msg.event);
        }
        else
        {
            log$("Unknown message!");
        }
    }

    return ipc_component_run(self);
}
