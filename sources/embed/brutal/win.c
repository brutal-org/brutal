#include <brutal/alloc.h>
#include <embed/win.h>
#include <protos/wm.h>

void embed_win_init(UiWin *self, MRect bound)
{
    IpcCap server = ipc_component_require(ipc_component_self(), IPC_WM_SERVER_PROTO);

    WmClientProps props = {
        .bound = bound,
        .type = self->type,
        .flags = self->flags,
    };

    wm_server_create_rpc(ipc_component_self(), server, &props, &self->embed.client, alloc_global());

    BalFb surface;
    wm_client_surface_rpc(ipc_component_self(), self->embed.client, &surface, alloc_global());
    bal_fb_map(&surface);

    self->embed.surface = surface;
}

void embed_win_deinit(UiWin *self)
{
    bal_fb_deinit(&self->embed.surface);

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
    return bal_fb_buf(&self->embed.surface);
}
