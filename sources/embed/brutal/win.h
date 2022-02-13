#pragma once

#include <bal/hw.h>

struct _EmbedWin
{
    IpcCap wm_client;
    IpcCap input_sink;
    BalFb surface;
};
