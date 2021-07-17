#pragma once

#include <brutal/base/attributes.h>
#include <brutal/base/std.h>

#define FOREACH_SYSCALLS(SYSCALL) \
    SYSCALL(NOOP)                 \
    SYSCALL(LOG)                  \
    SYSCALL(SPACE)                \
    SYSCALL(MOBJ)                 \
    SYSCALL(MAP)                  \
    SYSCALL(ALLOC)                \
    SYSCALL(UNMAP)                \
    SYSCALL(TASK)                 \
    SYSCALL(START)                \
    SYSCALL(EXIT)                 \
    SYSCALL(SEND)                 \
    SYSCALL(RECV)                 \
    SYSCALL(IRQ)                  \
    SYSCALL(DROP)                 \
    SYSCALL(CLOSE)

typedef enum
{
#define ITER(SYSCALL) BR_SC_##SYSCALL,
    FOREACH_SYSCALLS(ITER)
#undef ITER

        BR_SYSCALL_COUNT
} BrSyscall;

typedef enum
{
    BR_SUCCESS,

    BR_BAD_ADDRESS,
    BR_OUT_OF_MEMORY,
    BR_BAD_EXE_FORMAT,
    BR_BAD_ARGUMENTS,
    BR_BAD_CAPABILITY,
    BR_BAD_HANDLE,
    BR_NOT_IMPLEMENTED,
    BR_BAD_SYSCALL,
    BR_CHANNEL_FULL,
    BR_CHANNEL_EMPTY,
} BrResult;

typedef uint64_t BrArg;

typedef uint64_t BrHandle;

typedef BrHandle BrSpace;

#define BR_SPACE_ERROR ((BrSpace)-1)
#define BR_SPACE_SELF ((BrSpace)-2)

typedef enum
{
    BR_SPACE_NONE = 0,
} BrSpaceFlags;

typedef BrHandle BrMObj;

#define BR_MOBJ_ERROR ((BrMObj)-1)

typedef enum
{
    BR_MOBJ_NONE = 0,

    BR_MOBJ_PMM = 1 << 0,
} BrMObjFlags;

typedef enum
{
    BR_MEM_NONE = 0,

    BR_MEM_WRITABLE = 1 << 0,
    BR_MEM_USER = 1 << 1,
} BrMemFlags;

typedef BrHandle BrTask;

#define BR_TASK_ERROR ((BrTask)-1)
#define BR_TASK_SELF ((BrTask)-2)
#define BR_TASK_INIT ((BrTask)-3)

typedef enum
{
    BR_TASK_NONE = 0,

    BR_TASK_USER = 1 << 0,
} BrTaskFlags;

typedef uint64_t BrTimeout;

typedef struct PACKED
{
    BrTask sender;
    size_t size;
    BrHandle handle;
} BrMessageHeader;

typedef struct PACKED
{
    BrMessageHeader header;
    uint8_t data[512 - sizeof(BrMessageHeader)];
} BrMessage;

_Static_assert(sizeof(BrMessage) == 512, "");

typedef enum
{
    BR_IPC_NONE = 0,

    BR_IPC_BLOCK = 1 << 0,
} BrIpcFlags;

typedef uint64_t BrIrq;

typedef enum
{
    BR_IRQ_NONE = 0,

    BR_IRQ_BIND = 1 << 0,
    BR_IRQ_UNBIND = 1 << 1,
} BrIrqFlags;

typedef enum
{
    BR_CAP_NONE,

    BR_CAP_IRQ = 1 << 0,
    BR_CAP_PMM = 1 << 1,
    BR_CAP_LOG = 1 << 2,
    BR_CAP_TASK = 1 << 3,

    BR_CAP_ALL = 0xffffffff,
} BrCap;

typedef struct
{
    uintptr_t arg1;
    uintptr_t arg2;
    uintptr_t arg3;
    uintptr_t arg4;
    uintptr_t arg5;
} BrTaskArgs;
