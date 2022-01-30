#include <brutal/alloc.h>
#include <ipc/ipc.h>
#include <protos/bbus.h>
#include <protos/hw.h>
#include <protos/input.h>
#include "wm/display.h"
#include "wm/wm.h"

/* --- Input Sink Protocol -------------------------------------------------- */

static InputError input_sink_handle_handler(void *ctx, UiEvent const *req, bool *resp, Alloc *)
{
    Wm *wm = ctx;
    wm_dispatch(wm, *req);

    *resp = true;
    return IPC_SUCCESS;
}

static InputSinkVTable _input_sink_vtable = {
    input_sink_handle_handler,
};

/* --- Entry Point ---------------------------------------------------------- */

int ipc_component_main(IpcComponent *self)
{
    IpcCap bbus_server = ipc_component_require(self, IPC_BBUS_SERVER_PROTO);
    IpcCap hw_display = ipc_component_require(self, IPC_HW_DISPLAY_PROTO);

    DisplayMode display_mode;
    hw_display_current(self, hw_display, &display_mode, alloc_global());

    BrHandle display_handle;
    hw_display_buffer(self, hw_display, &display_handle, alloc_global());

    BalMem display_mem;
    bal_mem_init_mobj(&display_mem, display_handle);

    WmDisplay display;
    wm_display_init(&display, display_mode, display_mem, alloc_global());

    Wm wm;
    wm_init(&wm, &display);

    IpcCap input_sink_cap = input_sink_provide(self, &_input_sink_vtable, &wm);
    bbus_server_expose(self, bbus_server, &input_sink_cap, alloc_global());
    wm_render(&wm);

    return ipc_component_run(self);
}
