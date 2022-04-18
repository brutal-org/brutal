#include <protos/window.h>
#include <brutal-alloc>
#include "window/client.h"
#include "window/server.h"

/* --- Window Client Protocol ----------------------------------------------- */

static WindowError window_client_show_handler(void *self, void *, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_show(client);
    *resp = true;
    return IPC_SUCCESS;
}

static WindowError window_client_hide_handler(void *self, void *, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_hide(client);
    *resp = true;
    return IPC_SUCCESS;
}

static WindowError window_client_close_handler(void *self, void *, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_close(client);
    *resp = true;
    return IPC_SUCCESS;
}

static WindowError window_client_flip_handler(void *self, MRectf const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_flip(client, *req);
    *resp = true;
    return IPC_SUCCESS;
}

static WindowError window_client_resize_handler(void *self, MRectf const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_resize(client, *req);
    *resp = true;
    return IPC_SUCCESS;
}

static WindowError window_client_surface_handler(void *self, void *, BalFb *resp, Alloc *)
{
    WmClient *client = self;
    bal_fb_dup(resp, &client->backbuffer);
    return IPC_SUCCESS;
}

static WindowError window_client_listen_handler(void *self, IpcCap const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    client->handle = *req;
    *resp = true;
    return IPC_SUCCESS;
}

static WindowClientVTable _window_client_vtable = {
    window_client_show_handler,
    window_client_hide_handler,
    window_client_close_handler,
    window_client_flip_handler,
    window_client_resize_handler,
    window_client_surface_handler,
    window_client_listen_handler,
};

/* --- Window Manager Client ------------------------------------------------ */

WmClient *wm_client_create(struct _WmServer *server, MRectf bound, UiWinType type)
{
    WmClient *self = alloc_make(alloc_global(), WmClient);

    ipc_object_init(base$(self), ipc_self(), alloc_global());

    self->server = server;
    self->bound = bound;
    self->type = type;

    gfx_surface_init(&self->frontbuffer, bound.width, bound.height, GFX_FMT_RGBA8888, alloc_global());
    bal_fb_init(&self->backbuffer, bound.width, bound.height, GFX_FMT_RGBA8888);
    bal_fb_map(&self->backbuffer);

    window_client_provide(base$(self), &_window_client_vtable);

    return self;
}

void wm_client_destroy(WmClient *self)
{
    bal_fb_deinit(&self->backbuffer);
    gfx_surface_deinit(&self->frontbuffer);
    ipc_object_deinit(base$(self));
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
    event_sink_dispatch_rpc(ipc_self(), self->handle, &event, &resp, alloc_global());
}
