#pragma once

#include <bal/abi.h>

typedef uint32_t IpcProto;

typedef struct
{
    BrAddr addr;
    IpcProto proto;
} IpcCap;
