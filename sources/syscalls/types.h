#pragma once

#include <brutal/base/attributes.h>
#include <brutal/base/std.h>

#define FOREACH_SYSCALLS(SYSCALL) \
    SYSCALL(NOOP)                 \
    SYSCALL(LOG)                  \
    SYSCALL(GROUP)                \
    SYSCALL(SPACE)                \
    SYSCALL(VMO)                  \
    SYSCALL(MAP)                  \
    SYSCALL(ALLOC)                \
    SYSCALL(UNMAP)                \
    SYSCALL(TASK)                 \
    SYSCALL(START)                \
    SYSCALL(EXIT)                 \
    SYSCALL(BLOCK)                \
    SYSCALL(SIGNAL)               \
    SYSCALL(BIND)                 \
    SYSCALL(LOCATE)               \
    SYSCALL(SEND)                 \
    SYSCALL(RECV)                 \
    SYSCALL(IRQ)                  \
    SYSCALL(DROP)                 \
    SYSCALL(PLEDGE)

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
} BrResult;

typedef uint64_t BrArg;

typedef uint64_t BrHandle;

typedef BrHandle BrGroupe;

#define BR_GROUPE_ERROR ((BrGroupe)-1)
#define BR_GROUPE_SELF ((BrGroupe)-2)

typedef enum
{
    BR_GROUP_NONE = 0,

    BR_GROUP_PERMEABLE = 1 << 0,
} BrGroupeFlags;

typedef BrHandle BrSpace;

#define BR_SPACE_ERROR ((BrSpace)-1)
#define BR_SPACE_SELF ((BrSpace)-2)

typedef enum
{
    BR_SPACE_NONE = 0,
} BrSpaceFlags;

typedef BrHandle BrVmo;

#define BR_VMO_ERROR ((BrVmo)-1)

typedef enum
{
    BR_VMO_NONE = 0,

    BR_VMO_PMM = 1 << 0,
} BrVmoFlags;

typedef enum
{
    BR_MEM_NONE = 0,

    BR_MEM_WRITABLE = 1 << 0,
    BR_MEM_USER = 1 << 1,
} BrMemFlags;

typedef BrHandle BrTask;

#define BR_TASK_ERROR ((BrTask)-1)
#define BR_TASK_SELF ((BrTask)-2)

typedef enum
{
    BR_TASK_NONE = 0,
} BrTaskFlags;

typedef struct PACKED
{

} BrBlocker;

typedef enum
{
    BR_BLOCK_NONE = 0,
} BrBlockFlags;

typedef uint64_t BrTimeout;

typedef enum
{
    BR_SIGNAL_NOOP = 0,

    BR_SIGNAL_KILL,
    BR_SIGNAL_PAUSE,
    BR_SIGNAL_CONTINUE,
} BrSignal;

typedef struct PACKED
{
    BrTask sender;
    size_t size;
    BrVmo vmo;
    uint8_t data[];
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

    BR_CAP_ALL = 0xffffffff,
} BrCap;
