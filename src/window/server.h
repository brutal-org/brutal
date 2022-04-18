#pragma once

#include <brutal-ui>
#include "window/client.h"
#include "window/display.h"

typedef struct _WmServer
{
    IpcObject base;

    IpcCap window_server;
    IpcCap input_sink;
    Fiber *render_fiber;

    GfxDirty display_dirty;
    WmDisplay *display;

    MVec2f mouse;
    Vec(WmClient *) clients;
} WmServer;

void wm_server_init(WmServer *self, WmDisplay *display);

void wm_server_deinit(WmServer *self);

void wm_server_dispatch(WmServer *self, UiEvent event);

void wm_server_should_layout(WmServer *self);

void wm_server_layout(WmServer *self);

void wm_server_should_render(WmServer *self, MRectf rect);

void wm_server_should_render_all(WmServer *self);

void wm_server_render(WmServer *self);

void wm_server_expose(WmServer *self, IpcCap bus);

WmClient *wm_server_client_at(WmServer *self, MVec2f vec);
