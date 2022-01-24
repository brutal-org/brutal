#pragma once

#include <bal/abi.h>

typedef uint64_t IpcProto;

typedef struct
{
    BrAddr addr;
    IpcProto proto;
} IpcCap;
