#pragma once

#include <brutal/time.h>

#define FOREACH_SYSCALLS(SYSCALL) \
    SYSCALL(LOG)                  \
    SYSCALL(MAP)                  \
    SYSCALL(UNMAP)                \
    SYSCALL(CREATE)               \
    SYSCALL(START)                \
    SYSCALL(EXIT)                 \
    SYSCALL(IPC)                  \
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

static inline const char *br_syscall_to_string(BrSyscall syscall)
{
    static const char *SYSCALL_NAMES[] = {
#define ITER(SYSCALL) #SYSCALL,
        FOREACH_SYSCALLS(ITER)
#undef ITER
    };

    if (syscall >= BR_SYSCALL_COUNT)
    {
        return "INVALID";
    }

    return SYSCALL_NAMES[syscall];
}

#define FOREACH_RESULTS(RESULT) \
    RESULT(SUCCESS)             \
    RESULT(INTERRUPTED)         \
    RESULT(TIMEOUT)             \
    RESULT(BAD_ADDRESS)         \
    RESULT(BAD_ARGUMENTS)       \
    RESULT(BAD_CAPABILITY)      \
    RESULT(BAD_HANDLE)          \
    RESULT(BAD_SYSCALL)         \
    RESULT(NOT_IMPLEMENTED)     \
    RESULT(OUT_OF_MEMORY)       \
    RESULT(WOULD_BLOCK)

typedef enum
{
#define ITER(RESULT) BR_##RESULT,
    FOREACH_RESULTS(ITER)
#undef ITER

        BR_RESULT_COUNT
} BrResult;

static inline const char *br_result_to_string(BrResult result)
{
    static const char *RESULT_NAMES[] = {
#define ITER(RESULT) #RESULT,
        FOREACH_RESULTS(ITER)
#undef ITER
    };

    if (result >= BR_RESULT_COUNT)
    {
        return "INVALID";
    }

    return RESULT_NAMES[result];
}

static inline Error br_result_to_error(BrResult result)
{
    switch (result)
    {
    case BR_SUCCESS:
        return ERR_SUCCESS;

    case BR_BAD_ADDRESS:
        return ERR_BAD_ADDRESS;

    case BR_OUT_OF_MEMORY:
        return ERR_OUT_OF_MEMORY;

    case BR_BAD_ARGUMENTS:
        return ERR_BAD_ARGUMENTS;

    case BR_BAD_CAPABILITY:
        return ERR_DENIED;

    case BR_BAD_HANDLE:
        return ERR_BAD_HANDLE;

    case BR_NOT_IMPLEMENTED:
        return ERR_NOT_IMPLEMENTED;

    case BR_BAD_SYSCALL:
        return ERR_BAD_SYSCALL;

    case BR_TIMEOUT:
        return ERR_TIMEOUT;

    case BR_WOULD_BLOCK:
        return ERR_WOULD_BLOCK;

    default:
        return ERR_UNDEFINED;
    }
}

typedef enum
{
    BR_OBJECT_NONE,

    BR_OBJECT_MEMORY,
    BR_OBJECT_DOMAIN,
    BR_OBJECT_SPACE,
    BR_OBJECT_TASK,
} BrObjectType;

typedef uint64_t BrArg;

typedef uint64_t BrHandle;

typedef BrHandle BrSpace;

#define BR_SPACE_ERROR ((BrSpace)-1)
#define BR_SPACE_SELF ((BrSpace)-2)

typedef enum
{
    BR_SPACE_NONE = 0,
} BrSpaceFlags;

typedef BrHandle BrMemObj;

#define BR_MEM_OBJ_ERROR ((BrMemObj)-1)
#define BR_MEM_OBJ_GINFO ((BrMemObj)-2)
#define BR_MEM_OBJ_LINFO ((BrMemObj)-3)

typedef enum
{
    BR_MEM_OBJ_NONE = 0,

    BR_MEM_OBJ_PMM = 1 << 0,
} BrMemObjFlags;

typedef enum
{
    BR_MEM_NONE = 0,

    BR_MEM_READABLE = 1 << 0,
    BR_MEM_WRITABLE = 1 << 1,
    BR_MEM_EXECUTABLE = 1 << 2,
    BR_MEM_USER = 1 << 3,
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

#define BR_TIMEOUT_INFINITY (-1)
typedef uint64_t BrTimeout;

typedef uint64_t BrProtocol;

typedef uint64_t BrMessageType;

typedef struct PACKED
{
    BrTask sender;
    BrProtocol protocol;
    BrMessageType type;
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
    BR_IPC_SEND = 1 << 1,
    BR_IPC_RECV = 1 << 2,
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

typedef struct
{
    Tick tick;
    TimeStamp time;
} BrGlobalInfo;

typedef struct
{
    BrTask task;
    StrFix128 name;
} BrLocalInfo;

typedef struct
{
    BrGlobalInfo *global_info;
    BrLocalInfo *local_info;
    BrTaskArgs args;
} BrStartInfo;
