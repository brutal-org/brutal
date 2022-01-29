#pragma once

#include <brutal/ds.h>
#include <brutal/gfx.h>
#include <brutal/math.h>
#include <ipc/ipc.h>

typedef struct _WmClient WmClient;
typedef struct _Wm Wm;

struct _WmClient
{
    GfxBuf buffer;

    Wm *wm;
};

WmClient wm_client_create(MRect bound);

void wm_client_destroy(WmClient *client);

struct _Wm
{
    GfxBuf screen;

    Vec(WmClient *) clients;
};

void wm_init(Wm *wm);

void wm_deinit(Wm *wm);

IpcCap wm_connect(Wm *wm, WmClient *client);
