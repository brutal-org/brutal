#include <embed/win.h>
#include <protos/event.h>
#include <protos/window.h>
#include <brutal-alloc>

/* --- Event Sink Protocol -------------------------------------------------- */

static EventError input_sink_dispatch_handler(void *ctx, UiEvent const *req, bool *resp, Alloc *)
{
    UiWin *win = ctx;
    UiEvent event = *req;
    ui_win_dispatch(win, &event);

    *resp = true;
    return IPC_SUCCESS;
}

static EventSinkVTable _input_sink_vtable = {
    input_sink_dispatch_handler,
};

/* --- Win Implementation --------------------------------------------------- */

void embed_win_init(UiWin *self, MRectf bound)
{
    EmbedWin *embed = &self->embed;
    ipc_object_init(base$(embed), ipc_self(), alloc_global());
    event_sink_provide(base$(embed), &_input_sink_vtable);

    IpcCap server = ipc_component_require(ipc_self(), IPC_WINDOW_SERVER_PROTO);

    WindowClientProps props = {
        .bound = bound,
        .type = self->type,
    };

    window_server_create_rpc(ipc_self(), server, &props, &embed->handle, alloc_global());

    BalFb surface;
    window_client_surface_rpc(ipc_self(), embed->handle, &surface, alloc_global());
    bal_fb_map(&surface);

    self->embed.surface = surface;

    bool success = false;
    IpcCap cap = ipc_object_cap(base$(embed), IPC_EVENT_SINK_PROTO);
    window_client_listen_rpc(ipc_self(), embed->handle, &cap, &success, alloc_global());
}

void embed_win_deinit(UiWin *self)
{
    bal_fb_deinit(&self->embed.surface);

    bool resp;
    window_client_close_rpc(ipc_self(), self->embed.handle, &resp, alloc_global());
}

void embed_win_show(UiWin *self)
{
    bool resp = false;
    window_client_show_rpc(ipc_self(), self->embed.handle, &resp, alloc_global());
}

void embed_win_hide(UiWin *self)
{
    bool resp = false;
    window_client_hide_rpc(ipc_self(), self->embed.handle, &resp, alloc_global());
}

bool embed_win_visible(UiWin *self)
{
    UNUSED(self);
    // TODO
    return true;
}

void embed_win_flip(UiWin *self, MRectf rect)
{
    bool resp = false;
    window_client_flip_rpc(ipc_self(), self->embed.handle, &rect, &resp, alloc_global());
}

GfxBuf embed_win_gfx(UiWin *self)
{
    return bal_fb_buf(&self->embed.surface);
}
