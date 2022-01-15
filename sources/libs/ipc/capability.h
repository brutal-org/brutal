#pragma once

#include <bal/abi.h>

typedef enum
{
    IPC_CAPABILITY_ADDR,
    IPC_CAPABILITY_RSDP,
} IpcCapabilityType;

typedef struct
{
    IpcCapabilityType type;

    union
    {
        struct
        {
            BrAddr address;
            uint64_t proto;
        } addr;

        uintptr_t rsdp;
    };
} IpcCapability;
