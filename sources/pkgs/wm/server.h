#pragma once

#include <brutal/ui.h>
#include "wm/client.h"
#include "wm/display.h"

typedef struct _WmServer
{
    IpcCap wm_server;
    IpcCap input_sink;

    WmLayout layout;
    GfxDirty dirty;
    WmDisplay *display;
    MVec2 mouse;
    Vec(WmClient *) clients;
} WmServer;

void wm_server_init(WmServer *self, WmDisplay *display);

void wm_server_deinit(WmServer *self);

void wm_server_dispatch(WmServer *self, UiEvent event);

void wm_server_should_layout(WmServer* self);

void wm_server_layout(WmServer* self);

void wm_server_should_render(WmServer *self, MRect rect);

void wm_server_should_render_all(WmServer *self);

void wm_server_render(WmServer *self);

void wm_server_expose(WmServer *self, IpcCap bus);
