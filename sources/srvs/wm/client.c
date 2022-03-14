#include <brutal/alloc.h>
#include <protos/wm.h>
#include "wm/client.h"
#include "wm/server.h"

/* --- Window Manager Client Protocol --------------------------------------- */

static WmError wm_client_show_handler(void *self, void *, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_show(client);
    *resp = true;
    return IPC_SUCCESS;
}

static WmError wm_client_hide_handler(void *self, void *, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_hide(client);
    *resp = true;
    return IPC_SUCCESS;
}

static WmError wm_client_close_handler(void *self, void *, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_close(client);
    *resp = true;
    return IPC_SUCCESS;
}

static WmError wm_client_flip_handler(void *self, MRectf const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_flip(client, *req);
    *resp = true;
    return IPC_SUCCESS;
}

static WmError wm_client_resize_handler(void *self, MRectf const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_resize(client, *req);
    *resp = true;
    return IPC_SUCCESS;
}

static WmError wm_client_surface_handler(void *self, void *, BalFb *resp, Alloc *)
{
    WmClient *client = self;
    bal_fb_dup(resp, &client->backbuffer);
    return IPC_SUCCESS;
}

static WmError wm_client_listen_handler(void *self, IpcCap const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    client->event_sink = *req;
    *resp = true;
    return IPC_SUCCESS;
}

static WmClientVTable _wm_client_vtable = {
    wm_client_show_handler,
    wm_client_hide_handler,
    wm_client_close_handler,
    wm_client_flip_handler,
    wm_client_resize_handler,
    wm_client_surface_handler,
    wm_client_listen_handler,
};

/* --- Window Manager Client ------------------------------------------------ */

WmClient *wm_client_create(struct _WmServer *server, MRectf bound, UiWinType type)
{
    WmClient *self = alloc_make(alloc_global(), WmClient);

    self->server = server;
    self->bound = bound;
    self->type = type;

    gfx_surface_init(&self->frontbuffer, bound.width, bound.height, GFX_FMT_RGBA8888, alloc_global());
    bal_fb_init(&self->backbuffer, bound.width, bound.height, GFX_FMT_RGBA8888);
    bal_fb_map(&self->backbuffer);

    self->wm_client = wm_client_provide(ipc_component_self(), &_wm_client_vtable, self);

    return self;
}

void wm_client_destroy(WmClient *self)
{
    ipc_component_revoke(ipc_component_self(), self->wm_client);

    bal_fb_deinit(&self->backbuffer);
    gfx_surface_deinit(&self->frontbuffer);
    alloc_free(alloc_global(), self);
}

GfxBuf wm_client_frontbuffer(WmClient *self)
{
    return gfx_surface_buf(&self->frontbuffer);
}

GfxBuf wm_client_backbuffer(WmClient *self)
{
    return bal_fb_buf(&self->backbuffer);
}

void wm_client_show(WmClient *self)
{
    self->visible = true;
    wm_server_should_render(self->server, self->bound);
}

void wm_client_hide(WmClient *self)
{
    self->visible = false;
    wm_server_should_render(self->server, self->bound);
}

void wm_client_close(WmClient *self)
{
    self->visible = false;
    wm_server_should_render(self->server, self->bound);
}

void wm_client_flip(WmClient *self, MRectf bound)
{
    gfx_ops_copy(wm_client_frontbuffer(self), wm_client_backbuffer(self), 0, 0);

    bound.pos = m_vec2f_add(bound.pos, self->bound.pos);
    wm_server_should_render(self->server, bound);
}

void wm_client_resize(WmClient *self, MRectf bound)
{
    wm_server_should_render(self->server, self->bound);
    self->bound = bound;
    wm_server_should_render(self->server, self->bound);
}

void wm_client_dispatch(WmClient *self, UiEvent event)
{
    if (ui_event_is_mouse(&event))
    {
        event.mouse.position = m_vec2f_sub(event.mouse.position, self->bound.pos);
    }

    bool resp;
    event_sink_dispatch_rpc(ipc_component_self(), self->event_sink, &event, &resp, alloc_global());
}
