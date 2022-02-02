#include <brutal/alloc.h>
#include <ipc/ipc.h>
#include <protos/bus.h>
#include <protos/event.h>
#include <protos/hw.h>
#include "wm/display.h"
#include "wm/server.h"

/* --- Input Sink Protocol -------------------------------------------------- */

static EventError input_sink_dispatch_handler(void *ctx, UiEvent const *req, bool *resp, Alloc *)
{
    WmServer *wm = ctx;
    wm_server_dispatch(wm, *req);

    *resp = true;
    return IPC_SUCCESS;
}

static EventSinkVTable _input_sink_vtable = {
    input_sink_dispatch_handler,
};

/* --- Entry Point ---------------------------------------------------------- */

int ipc_component_main(IpcComponent *self)
{
    IpcCap bus_server = ipc_component_require(self, IPC_BUS_SERVER_PROTO);
    IpcCap hw_display = ipc_component_require(self, IPC_HW_DISPLAY_PROTO);

    BalFb surface;
    hw_display_surface_rpc(self, hw_display, &surface, alloc_global());

    WmDisplay display;
    wm_display_init(&display, surface, alloc_global());

    WmServer server;
    wm_server_init(&server, &display);

    IpcCap input_sink_cap = event_sink_provide(self, &_input_sink_vtable, &server);

    bus_server_expose_rpc(self, bus_server, &input_sink_cap, alloc_global());
    bus_server_expose_rpc(self, bus_server, &server.capability, alloc_global());

    wm_server_dirty_all(&server);
    wm_server_render(&server);

    return ipc_component_run(self);
}
