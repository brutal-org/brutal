#include <protos/hw.h>
#include <protos/system.h>
#include <brutal-alloc>
#include "window/server.h"

int ipc_component_main(IpcComponent *self)
{
    IpcCap bus_server = ipc_component_require(self, IPC_SYSTEM_SERVER_PROTO);
    IpcCap hw_display = ipc_component_require(self, IPC_HW_DISPLAY_PROTO);

    BalFb surface = {};
    hw_display_surface_rpc(self, hw_display, &surface, alloc_global());

    WmDisplay display = {};
    wm_display_init(&display, surface, alloc_global());

    WmServer server = {};
    wm_server_init(&server, &display);
    wm_server_expose(&server, bus_server);

    wm_server_should_render_all(&server);
    wm_server_render(&server);

    return ipc_component_run(self);
}
