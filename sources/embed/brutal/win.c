#include <brutal/alloc.h>
#include <embed/win.h>
#include <protos/wm.h>

void embed_win_init(UiWin *self, MRect bound)
{
    IpcCap server = ipc_component_require(ipc_component_self(), IPC_WM_SERVER_PROTO);

    WmServerCreateRequest req = {
        .bound = bound,
    };

    wm_server_create_rpc(ipc_component_self(), server, &req, &self->embed.client, alloc_global());

    Surface surface;
    wm_client_surface_rpc(ipc_component_self(), self->embed.client, &surface, alloc_global());

    self->embed.mode = surface.mode;
    bal_mem_init_mobj(&self->embed.mem, surface.handle);
}

void embed_win_deinit(UiWin *self)
{
    bal_mem_deinit(&self->embed.mem);

    bool resp;
    wm_client_close_rpc(ipc_component_self(), self->embed.client, &resp, alloc_global());
}

void embed_win_show(UiWin *self)
{
    bool resp = false;
    wm_client_show_rpc(ipc_component_self(), self->embed.client, &resp, alloc_global());
}

void embed_win_hide(UiWin *self)
{
    bool resp = false;
    wm_client_hide_rpc(ipc_component_self(), self->embed.client, &resp, alloc_global());
}

bool embed_win_visible(UiWin *self)
{
    UNUSED(self);
    // TODO
    return true;
}

void embed_win_flip(UiWin *self, MRect rect)
{
    bool resp = false;
    wm_client_flip_rpc(ipc_component_self(), self->embed.client, &rect, &resp, alloc_global());
}

GfxBuf embed_win_gfx(UiWin *self)
{
    EmbedWin *embed = &self->embed;

    return (GfxBuf){
        .width = embed->mode.width,
        .height = embed->mode.height,
        .pitch = embed->mode.pitch,
        .fmt = embed->mode.format,
        .buf = embed->mem.buf,
    };
}
