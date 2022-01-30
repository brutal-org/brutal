#include <brutal/alloc.h>
#include <ipc/ipc.h>
#include <protos/bbus.h>
#include <protos/hw.h>
#include <protos/input.h>
#include "wm/display.h"

/* --- Input Sink Protocol -------------------------------------------------- */

static InputError input_sink_handle_handler(void *, UiEvent const *req, bool *resp, Alloc *)
{
    log$("EVENT {}!", req->type);
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

    IpcCap input_sink_cap = input_sink_provide(self, &_input_sink_vtable, nullptr);

    bbus_server_expose(self, bbus_server, &input_sink_cap, alloc_global());

    DisplayMode display_mode;
    hw_display_current(self, hw_display, &display_mode, alloc_global());

    BrHandle display_handle;
    hw_display_buffer(self, hw_display, &display_handle, alloc_global());

    BalMem display_mem;
    bal_mem_init_mobj(&display_mem, display_handle);

    WmDisplay display;
    wm_display_init(&display, display_mode, display_mem, alloc_global());

    Gfx *gfx = wm_display_begin(&display);

    gfx_clear(gfx, GFX_GAINSBORO);
    gfx_color(gfx, GFX_BLACK);
    gfx_fill(gfx, gfx_paint_fill(GFX_RED));
    gfx_fill_text(gfx, m_vec2(16, 24), str$("Hello, world!"), gfx_font_builtin());
    gfx_fill_rect(gfx, m_rect(16, 16, 32, 32), 0);

    wm_display_end(&display);

    return ipc_component_run(self);
}
