#pragma once

#include <bal/hw.h>
#include <ipc/ipc.h>

struct _EmbedWin
{
    IpcObject base;
    IpcCap handle; // to the object on the server side

    BalFb surface;
};
