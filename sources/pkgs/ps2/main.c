#include <bal/abi.h>
#include <bal/hw.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/ui.h>
#include <hw/ps2/ps2.h>
#include <ipc/ipc.h>
#include <protos/event.h>
#include "bal/abi/types.h"
#include "brutal/ui/event.h"

void ps2_keyboard_callback(UiEvent ev, IpcCap*input_sink)
{
    bool handled = false;

    log$("ps2kb: key={} motion={}", kbkey_to_str(ev.keyboard.key), (ev.type == UI_EVENT_KEYBOARD_DOWN) ? "down" : "up");

    event_sink_dispatch_rpc(ipc_component_self(), *input_sink, &ev, &handled, alloc_global());
}

void ps2_mouse_callback(UiEvent ev, IpcCap *input_sink)
{
    bool handled = false;

    event_sink_dispatch_rpc(ipc_component_self(), *input_sink, &ev, &handled, alloc_global());
}

int ipc_component_main(IpcComponent *self)
{
    Ps2 ps2 = {};
    IpcCap input_sink = ipc_component_require(self, IPC_EVENT_SINK_PROTO);
    init_ps2(&ps2);
    init_ps2_keyboard(&ps2, (Ps2KeyboardCallback)&ps2_keyboard_callback, &input_sink);
    init_ps2_mouse(&ps2, (Ps2MouseCallback)&ps2_mouse_callback, &input_sink);

    return ipc_component_run(self);
}
