#pragma once

#include <brutal/base/range.h>
#include <brutal/time.h>

#define FOREACH_SYSCALLS(SYSCALL) \
    SYSCALL(LOG)                  \
    SYSCALL(MAP)                  \
    SYSCALL(UNMAP)                \
    SYSCALL(CREATE)               \
    SYSCALL(START)                \
    SYSCALL(EXIT)                 \
    SYSCALL(IPC)                  \
    SYSCALL(DROP)                 \
    SYSCALL(CLOSE)                \
    SYSCALL(BIND)                 \
    SYSCALL(UNBIND)               \
    SYSCALL(ACK)                  \
    SYSCALL(STAT)                 \
    SYSCALL(IN)                   \
    SYSCALL(OUT)

typedef enum
{
#define ITER(SYSCALL) BR_SC_##SYSCALL,
    FOREACH_SYSCALLS(ITER)
#undef ITER

        BR_SYSCALL_COUNT
} BrSyscall;

#define FOREACH_RESULTS(RESULT) \
    RESULT(SUCCESS)             \
    RESULT(INTERRUPTED)         \
    RESULT(TIMEOUT)             \
    RESULT(ALREADY_BINDED)      \
    RESULT(ALREADY_ACK)         \
    RESULT(NOT_BINDED)          \
    RESULT(BAD_ADDRESS)         \
    RESULT(BAD_ARGUMENTS)       \
    RESULT(BAD_CAPABILITY)      \
    RESULT(BAD_HANDLE)          \
    RESULT(BAD_ID)              \
    RESULT(BAD_SYSCALL)         \
    RESULT(NOT_IMPLEMENTED)     \
    RESULT(OUT_OF_MEMORY)       \
    RESULT(CHANNEL_FULL)        \
    RESULT(CHANNEL_EMPTY)       \
    RESULT(WOULD_BLOCK)

typedef enum
{
#define ITER(RESULT) BR_##RESULT,
    FOREACH_RESULTS(ITER)
#undef ITER

        BR_RESULT_COUNT
} BrResult;

typedef enum
{
    BR_OBJECT_NONE,
    BR_OBJECT_ANY,

    BR_OBJECT_MEMORY,
    BR_OBJECT_DOMAIN,
    BR_OBJECT_SPACE,
    BR_OBJECT_TASK,
} BrObjectType;

typedef uint64_t BrArg;

#define BR_ID_NIL ((BrId)0)

typedef uint64_t BrId;

#define BR_HANDLE_NIL ((BrHandle)0)

typedef uint64_t BrHandle;

typedef BrHandle BrSpace;

#define BR_SPACE_NIL ((BrSpace)0)
#define BR_SPACE_ERROR ((BrSpace)-1)
#define BR_SPACE_SELF ((BrSpace)-2)

typedef enum
{
    BR_SPACE_NONE = 0,
} BrSpaceFlags;

typedef BrHandle BrMemObj;

#define BR_MEM_OBJ_NIL ((BrMemObj)0)
#define BR_MEM_OBJ_ERROR ((BrMemObj)-1)
#define BR_MEM_OBJ_GINFO ((BrMemObj)-2)
#define BR_MEM_OBJ_LINFO ((BrMemObj)-3)

typedef enum
{
    BR_MEM_OBJ_NONE = 0,

    BR_MEM_OBJ_PMM = 1 << 0,
    BR_MEM_OBJ_LOWER = 1 << 1,
} BrMemObjFlags;

#define BR_MEM_ZERO ((BrMemFlags)0)
#define BR_MEM_READABLE ((BrMemFlags)(1 << 0))
#define BR_MEM_WRITABLE ((BrMemFlags)(1 << 1))
#define BR_MEM_EXECUTABLE ((BrMemFlags)(1 << 2))
#define BR_MEM_USER ((BrMemFlags)(1 << 3))

typedef uint32_t BrMemFlags;

typedef BrHandle BrTask;

#define BR_TASK_NIL ((BrTask)0)
#define BR_TASK_ERROR ((BrTask)-1)
#define BR_TASK_SELF ((BrTask)-2)
#define BR_TASK_INIT ((BrTask)-3)
#define BR_TASK_IRQ ((BrTask)-4)

#define BR_TASK_ZERO (BrTaskFlags)(0)
#define BR_TASK_USER (BrTaskFlags)(1 << 0)

typedef uint8_t BrTaskFlags;

#define BR_DEADLINE_INFINITY ((BrDeadline)-1)
typedef uint64_t BrDeadline;

#define BR_TIMEOUT_INFINITY ((BrTimeout)-1)
typedef uint64_t BrTimeout;

#define BR_MSG_ARG_COUNT (5)

#define BR_MSG_ZERO (0)
#define BR_MSG_HND(I) (1 << (I))

typedef uint32_t BrMsgFlags;

typedef BrHandle BrIrq;

typedef enum
{
    BR_EVENT_INVALID,

    BR_EVENT_IRQ,
} BrEventType;

typedef struct
{
    BrEventType type;
    union
    {
        BrIrq irq;
    };
} BrEvent;

typedef struct
{
    BrId from;
    BrMsgFlags flags;
    uint32_t seq;
    uint32_t prot;

#define BR_MSG_ERROR (uint32_t)(-1)
    uint32_t type;

    union
    {
        BrArg args[BR_MSG_ARG_COUNT];
        BrEvent event;
    };
} BrMsg;

_Static_assert(sizeof(BrMsg) == 64, "");

#define BR_IPC_ZERO ((BrIpcFlags)(0))
#define BR_IPC_BLOCK ((BrIpcFlags)(1 << 0))
#define BR_IPC_SEND ((BrIpcFlags)(1 << 1))
#define BR_IPC_RECV ((BrIpcFlags)(1 << 2))

typedef uint32_t BrIpcFlags;

#define BR_CAP_ZERO (0)
#define BR_CAP_IRQ (1 << 0)
#define BR_CAP_PMM (1 << 1)
#define BR_CAP_LOG (1 << 2)
#define BR_CAP_TASK (1 << 3)
#define BR_CAP_IO (1 << 4)
#define BR_CAP_ALL (0xffffffff)

typedef uint32_t BrCap;

typedef struct
{
    Tick tick;
    TimeStamp time;
} BrGlobals;

typedef struct
{
    BrTask task;
    StrFix128 name;
} BrLocals;

typedef enum
{
    BR_START_NONE,
    BR_START_CMAIN,
    BR_START_ARGS,
    BR_START_HANDOVER,
} BrStartType;

typedef struct
{
    BrStartType type;

    uintptr_t arg1;
    uintptr_t arg2;
    uintptr_t arg3;
    uintptr_t arg4;
    uintptr_t arg5;
} BrTaskArgs;

typedef struct
{
    StrFix128 name;
    BrCap caps;
    bool stopped;
    bool started;
    bool blocked;
    BrTask id;
} BrInfoTask;

typedef struct
{
    size_t domain_object_count;
} BrInfoDomain;

typedef struct
{
    USizeRange range;
} BrInfoMemObj;

typedef struct
{
    BrSpaceFlags flags;
} BrInfoSpace;

typedef struct
{
    BrObjectType type;

    union
    {
        BrInfoMemObj memobj;
        BrInfoDomain domainobj;
        BrInfoSpace spaceobj;
        BrInfoTask taskobj;
    };
} BrHandleInfo;
