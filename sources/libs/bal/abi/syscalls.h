#pragma once

#include <bal/abi/types.h>

// clang-format off

#ifdef __kernel__
#    define SYSCALL(NAME) BrResult sys_##NAME
#else
#    define SYSCALL(NAME) BrResult br_##NAME
SYSCALL(syscall) (BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5);
#endif

typedef struct
{
    char const *message;
    size_t size;
} BrLogArgs;

SYSCALL(log) (BrLogArgs* args);

typedef struct
{
    BrHandle space;
    BrHandle memory;

    size_t offset;
    size_t size;
    uintptr_t vaddr;
    BrMemoryFlags flags;
} BrMapArgs;

SYSCALL(map) (BrMapArgs* args);

typedef struct
{
    BrHandle space;

    uintptr_t vaddr;
    size_t size;
} BrUnmapArgs;

SYSCALL(unmap) (BrUnmapArgs* args);

typedef struct
{
    BrHandle space;
    BrRight rights;
    BrTaskFlags flags;
} BrTaskProps;

typedef struct
{
    uintptr_t addr;
    size_t size;
    BrMemoryFlags flags;
} BrMemoryProps;

typedef struct
{
    BrSpaceFlags flags;
} BrSpaceProps;

typedef struct
{
    BrId id;
    BrType type;
    BrHandle handle;

    union
    {
        BrTaskProps task;
        BrMemoryProps memory;
        BrSpaceProps space;
    };
} BrCreateArgs;

SYSCALL(create) (BrCreateArgs* args);

typedef struct
{
    BrHandle handle;
    uintptr_t ip;
    uintptr_t sp;
    BrTaskArgs args;
} BrStartArgs;

SYSCALL(start) (BrStartArgs* args);

typedef struct
{
    BrHandle handle;
    uintptr_t result;
} BrExitArgs;

SYSCALL(exit) (BrExitArgs* args);

#define BR_IPC_BLOCK ((BrIpcFlags)(1 << 0))
#define BR_IPC_SEND ((BrIpcFlags)(1 << 1))
#define BR_IPC_RECV ((BrIpcFlags)(1 << 2))

typedef uint32_t BrIpcFlags;

typedef struct
{
    BrId to;
    BrMsg msg;
    BrDeadline deadline;
    BrIpcFlags flags;
} BrIpcArgs;

SYSCALL(ipc) (BrIpcArgs* args);

typedef struct
{
    BrHandle handle;
    BrRight cap;
} BrDropArgs;

SYSCALL(drop) (BrDropArgs* args);

typedef struct
{
    BrHandle handle;
} BrCloseArgs;

SYSCALL(close) (BrCloseArgs* args);

typedef struct
{
    BrEvent event;
} BrBindArgs;

SYSCALL(bind) (BrBindArgs* args);

typedef struct
{
    BrEvent event;
} BrUnbindArgs;

SYSCALL(unbind) (BrUnbindArgs* args);

typedef struct
{
    BrEvent event;
} BrAckArgs;

SYSCALL(ack) (BrAckArgs* args);

typedef struct
{
    BrRight rights;
    bool stopped;
    bool started;
    bool blocked;
} BrTaskInfos;

typedef struct
{
    size_t domain_object_count;
} BrDomainInfos;

typedef struct
{
    USizeRange range;
} BrMemoryInfos;

typedef struct
{
    BrSpaceFlags flags;
} BrSpaceInfos;

typedef struct
{
    BrId id;
    BrHandle handle;
    BrType type;

    union
    {
        BrMemoryInfos memory;
        BrDomainInfos domain;
        BrSpaceInfos space;
        BrTaskInfos task;
    };
} BrInspectArgs;

SYSCALL(inspect)(BrInspectArgs* args);

typedef struct
{
    uint16_t port;
    uint32_t data;
    int size;
}BrIoArgs;

SYSCALL(in)(BrIoArgs* args);

SYSCALL(out)(BrIoArgs* args);

// clang-format on
