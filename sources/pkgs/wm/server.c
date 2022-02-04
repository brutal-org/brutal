#include <brutal/alloc.h>
#include <protos/bus.h>
#include <protos/wm.h>
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

/* --- Window Manager Server Protocol --------------------------------------- */

WmError wm_server_create_handler(void *self, WmClientProps const *req, IpcCap *resp, Alloc *)
{
    WmServer *server = self;

    WmClient *client = wm_client_create(server, req->bound, req->type, req->flags);
    vec_push(&server->clients, client);
    *resp = client->wm_client;

    return IPC_SUCCESS;
}

WmServerVTable _wm_server_vtable = {
    wm_server_create_handler,
};

/* --- Window Manager Server ------------------------------------------------ */

void wm_server_init(WmServer *self, WmDisplay *display)
{
    self->display = display;
    self->mouse = m_vec2(0, 0);
    gfx_dirty_init(&self->dirty, alloc_global());
    vec_init(&self->clients, alloc_global());

    self->wm_server = wm_server_provide(ipc_component_self(), &_wm_server_vtable, self);
    self->input_sink = event_sink_provide(ipc_component_self(), &_input_sink_vtable, self);
}

void wm_server_deinit(WmServer *self)
{
    ipc_component_revoke(ipc_component_self(), self->wm_server);
    ipc_component_revoke(ipc_component_self(), self->input_sink);

    vec_deinit(&self->clients);
    gfx_dirty_deinit(&self->dirty);
}

void wm_server_dispatch(WmServer *self, UiEvent event)
{
    if (event.type == UI_EVENT_MOUSE_MOVE)
    {
        MRect mouse_bound = {
            .pos = self->mouse,
            .size = m_vec2(32, 32),
        };

        wm_server_should_render(self, mouse_bound);

        self->mouse = m_vec2_add(self->mouse, event.mouse.offset);

        mouse_bound = (MRect){
            .pos = self->mouse,
            .size = m_vec2(32, 32),
        };

        wm_server_should_render(self, mouse_bound);
        wm_server_render(self);
    }
}

static void wm_server_render_cursor(WmServer *self, Gfx *gfx)
{
    gfx_push(gfx);
    gfx_origin(gfx, self->mouse);
    gfx_fill(gfx, gfx_paint_fill(GFX_BLACK));
    gfx_fill_svg(gfx, str$("M 0 0 L 16 12.279 L 9.049 13.449 L 13.374 22.266 L 9.778 24 L 5.428 15.121 L -0 19.823 Z"));
    gfx_pop(gfx);
}

static void wm_server_render_clients(WmServer *self, Gfx *gfx)
{
    vec_foreach_v(client, &self->clients)
    {
        gfx_fill(gfx, gfx_paint_image(wm_client_backbuffer(client), gfx_buf_bound(wm_client_backbuffer(client))));

        if (client->flags & UI_WIN_BORDERLESS)
        {
            gfx_fill_rect(gfx, client->bound, 0);
        }
        else
        {
            gfx_fill_rect(gfx, client->bound, 8);
        }
    }
}

void wm_server_should_render(WmServer *self, MRect rect)
{
    gfx_dirty_rect(&self->dirty, rect);
}

void wm_server_should_render_all(WmServer *self)
{
    wm_server_should_render(self, wm_display_bound(self->display));
}

void wm_server_render(WmServer *self)
{
    Gfx *gfx = wm_display_begin(self->display);

    gfx_dirty_foreach(dirty, &self->dirty)
    {
        gfx_push(gfx);
        gfx_clip(gfx, dirty);

        gfx_clear(gfx, GFX_GAINSBORO);

        wm_server_render_clients(self, gfx);
        wm_server_render_cursor(self, gfx);

        gfx_fill(gfx, gfx_paint_fill(gfx_color_rand(50)));
        gfx_fill_rect(gfx, dirty, 0);

        gfx_pop(gfx);

        wm_display_flip(self->display, dirty);
    }

    wm_display_end(self->display);

    gfx_dirty_clear(&self->dirty);
}

void wm_server_expose(WmServer *self, IpcCap bus_server)
{
    bus_server_expose_rpc(ipc_component_self(), bus_server, &self->input_sink, alloc_global());
    bus_server_expose_rpc(ipc_component_self(), bus_server, &self->wm_server, alloc_global());
}
