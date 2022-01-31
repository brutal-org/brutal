#include <brutal/alloc.h>
#include <protos/wm.h>
#include "wm/server.h"

/* --- Window Manager Server Protocol --------------------------------------- */

WmError wm_server_create_handler(void *self, WmServerCreateRequest const *req, IpcCap *resp, Alloc *)
{
    WmServer *server = self;

    WmClient *client = wm_client_create(server, req->bound);
    vec_push(&server->clients, client);
    *resp = client->capability;

    return IPC_SUCCESS;
}

WmServerVTable _wm_server_vtable = {
    wm_server_create_handler,
};

/* --- Window Manager Server ------------------------------------------------ */

void wm_server_init(WmServer *self, WmDisplay *display)
{
    self->display = display;
    self->mouse = m_vec2(64, 64);
    vec_init(&self->clients, alloc_global());

    self->capability = wm_server_provide(ipc_component_self(), &_wm_server_vtable, self);
}

void wm_server_deinit(WmServer *self)
{
    ipc_component_revoke(ipc_component_self(), self->capability);
    vec_deinit(&self->clients);
}

void wm_server_dispatch(WmServer *self, UiEvent event)
{
    if (event.type == UI_EVENT_MOUSE_MOVE)
    {
        self->mouse = m_vec2_add(self->mouse, event.mouse.offset);
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
    gfx_fill(gfx, gfx_paint_fill(GFX_MAGENTA));
    vec_foreach_v(client, &self->clients)
    {
        gfx_fill_rect(gfx, client->bound, 8);
    }
}

void wm_server_dirty(MAYBE_UNUSED WmServer *self)
{
}

void wm_server_render(WmServer *self)
{
    Gfx *gfx = wm_display_begin(self->display);

    gfx_clear(gfx, GFX_GAINSBORO);

    gfx_color(gfx, GFX_BLACK);
    gfx_fill(gfx, gfx_paint_fill(GFX_BLACK));

    wm_server_render_clients(self, gfx);
    wm_server_render_cursor(self, gfx);

    wm_display_end(self->display);
}
