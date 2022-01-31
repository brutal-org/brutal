#pragma once

#include <bal/hw.h>
#include <ipc/ipc.h>
#include <protos/surface.h>

struct _EmbedWin
{
    IpcCap client;
    SurfaceMode mode;
    BalMem mem;
};
