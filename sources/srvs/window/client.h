#pragma once

#include <bal/hw.h>
#include <bruwutal/gfx.h>
#include <bruwutal/math.h>
#include <bruwutal/ui.h>
#include <ipc/ipc.h>

typedef struct
{
    UiWinType type;

    IpcCap wm_client;
    IpcCap event_sink;

    bool visible;
    MRectf bound;

    GfxSurface frontbuffer;
    BalFb backbuffer;

    struct _WmServer *server;
} WmClient;

WmClient *wm_client_create(struct _WmServer *server, MRectf bound, UiWinType type);

void wm_client_destroy(WmClient *self);

GfxBuf wm_client_frontbuffer(WmClient *self);

GfxBuf wm_client_backbuffer(WmClient *self);

void wm_client_show(WmClient *self);

void wm_client_hide(WmClient *self);

void wm_client_close(WmClient *self);

void wm_client_flip(WmClient *self, MRectf bound);

void wm_client_resize(WmClient *self, MRectf bound);

void wm_client_dispatch(WmClient *self, UiEvent event);
