#pragma once

#include <bal/abi.h>

typedef uint32_t IpcProto;

typedef Slice(IpcProto) IpcProtos;

typedef struct
{
    BrAddr addr;
    IpcProto proto;
} IpcCap;

typedef Slice(IpcCap) IpcCaps;
