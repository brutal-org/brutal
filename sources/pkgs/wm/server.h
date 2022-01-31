#pragma once

#include <brutal/ui.h>
#include "wm/client.h"
#include "wm/display.h"

typedef struct _WmServer
{
    IpcCap capability;
    WmDisplay *display;
    MVec2 mouse;
    Vec(WmClient *) clients;
} WmServer;

void wm_server_init(WmServer *self, WmDisplay *display);

void wm_server_deinit(WmServer *self);

void wm_server_dispatch(WmServer *self, UiEvent event);

void wm_server_dirty(WmServer *self);

void wm_server_render(WmServer *self);
