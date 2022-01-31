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

static WmError wm_client_flip_handler(void *self, MRect const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_flip(client, *req);
    *resp = true;
    return IPC_SUCCESS;
}

static WmError wm_client_move_handler(void *self, MRect const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    wm_client_move(client, *req);
    *resp = true;
    return IPC_SUCCESS;
}

static WmError wm_client_surface_handler(void *self, void *, Surface *resp, Alloc *)
{
    WmClient *client = self;

    *resp = (Surface){
        .mode = {
            client->frontbuffer.buf.width,
            client->frontbuffer.buf.height,
            client->frontbuffer.buf.pitch,
            client->frontbuffer.buf.fmt,
        },
        .handle = bal_dup(client->backbuffer.handle),
    };

    return IPC_SUCCESS;
}

static WmError wm_client_listen_handler(void *self, IpcCap const *req, bool *resp, Alloc *)
{
    WmClient *client = self;
    client->sink = *req;
    *resp = true;
    return IPC_SUCCESS;
}

static WmClientVTable _wm_client_vtable = {
    wm_client_show_handler,
    wm_client_hide_handler,
    wm_client_close_handler,
    wm_client_flip_handler,
    wm_client_move_handler,
    wm_client_surface_handler,
    wm_client_listen_handler,
};

/* --- Window Manager Client ------------------------------------------------ */

WmClient *wm_client_create(struct _WmServer *server, MRect bound)
{
    WmClient *self = alloc_make(alloc_global(), WmClient);

    self->server = server;
    self->bound = bound;

    gfx_dyn_buf_init(&self->frontbuffer, bound.width, bound.height, GFX_FMT_RGBA8888, alloc_global());
    bal_mem_init_size(&self->backbuffer, self->frontbuffer.buf.size);

    self->capability = wm_client_provide(ipc_component_self(), &_wm_client_vtable, self);

    return self;
}

void wm_client_destroy(WmClient *self)
{
    ipc_component_revoke(ipc_component_self(), self->capability);

    bal_mem_deinit(&self->backbuffer);
    gfx_dyn_buf_deinit(&self->frontbuffer);
    alloc_free(alloc_global(), self);
}

GfxBuf wm_client_frontbuffer(WmClient *self)
{
    return gfx_dyn_buf(&self->frontbuffer);
}

GfxBuf wm_client_backbuffer(WmClient *self)
{
    return (GfxBuf){
        self->frontbuffer.buf.width,
        self->frontbuffer.buf.height,
        self->frontbuffer.buf.pitch,
        self->frontbuffer.buf.fmt,
        .buf = self->backbuffer.buf,
        .size = self->backbuffer.len,
    };
}

void wm_client_show(WmClient *self)
{
    self->visible = true;
    wm_server_dirty(self->server);
}

void wm_client_hide(WmClient *self)
{
    self->visible = false;
    wm_server_dirty(self->server);
}

void wm_client_close(WmClient *self)
{
    self->visible = true;
    wm_server_dirty(self->server);
}

void wm_client_flip(WmClient *self, MAYBE_UNUSED MRect bound)
{
    gfx_buf_copy(wm_client_frontbuffer(self), wm_client_backbuffer(self), 0, 0);
}

void wm_client_move(WmClient *self, MRect bound)
{
    self->bound = bound;
    wm_server_dirty(self->server);
}

void wm_client_dispatch(WmClient *self, UiEvent event)
{
    bool resp;
    event_sink_dispatch_rpc(ipc_component_self(), self->sink, &event, &resp, alloc_global());
}
