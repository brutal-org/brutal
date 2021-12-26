#pragma once

#include <brutal/base/range.h>
#include <brutal/time.h>

/* --- Syscalls IDs --------------------------------------------------------- */

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
    SYSCALL(INSPECT)              \
    SYSCALL(IN)                   \
    SYSCALL(OUT)

typedef enum
{
#define ITER(SYSCALL) BR_SC_##SYSCALL,
    FOREACH_SYSCALLS(ITER)
#undef ITER

        BR_SYSCALL_COUNT
} BrSyscall;

typedef uint64_t BrArg;

/* --- Syscalls Results ----------------------------------------------------- */

#define FOREACH_RESULTS(RESULT) \
    RESULT(SUCCESS)             \
    RESULT(INTERRUPTED)         \
    RESULT(TIMEOUT)             \
    RESULT(ALREADY_BINDED)      \
    RESULT(ALREADY_ACK)         \
    RESULT(NOT_BINDED)          \
    RESULT(NOT_PERMITTED)       \
    RESULT(BAD_ADDRESS)         \
    RESULT(BAD_ARGUMENTS)       \
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

/* --- Kernels Objects ------------------------------------------------------ */

typedef uint64_t BrId;
#define BR_ID_NIL ((BrId)0)
#define BR_ID_ERR ((BrId)-1)
#define BR_ID_SUPER ((BrId)-3)
#define BR_ID_EVENT ((BrId)-4)

typedef uint64_t BrHandle;
#define BR_HANDLE_NIL ((BrHandle)0)
#define BR_HANDLE_ERR ((BrHandle)-1)
#define BR_HANDLE_SELF ((BrHandle)-2)

typedef enum
{
    BR_OBJECT_NONE,
    BR_OBJECT_ANY,

    BR_OBJECT_MEMORY,
    BR_OBJECT_DOMAIN,
    BR_OBJECT_SPACE,
    BR_OBJECT_TASK,
} BrType;

// Memory Objects

#define BR_MEM_READABLE ((BrMemoryFlags)(1 << 0))
#define BR_MEM_WRITABLE ((BrMemoryFlags)(1 << 1))
#define BR_MEM_EXECUTABLE ((BrMemoryFlags)(1 << 2))
#define BR_MEM_USER ((BrMemoryFlags)(1 << 3))
#define BR_MEM_LOWER ((BrMemoryFlags)(1 << 4))
#define BR_MEM_PMM ((BrMemoryFlags)(1 << 5))

typedef uint64_t BrMemoryFlags;

// Domain Objects

typedef uint64_t BrDomainFlags;

// Space Objects

typedef uint64_t BrSpaceFlags;

/// Task Objects

#define BR_TASK_USER (BrTaskFlags)(1 << 0)

typedef uint64_t BrTaskFlags;

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

    BrArg arg1;
    BrArg arg2;
    BrArg arg3;
    BrArg arg4;
    BrArg arg5;
} BrTaskArgs;

/* --- IPCs ----------------------------------------------------------------- */

#define BR_MSG_ZERO (0)
#define BR_MSG_HND(I) (1 << (I))

typedef uint32_t BrMsgFlags;

// Time

#define BR_DEADLINE_INFINITY ((BrDeadline)-1)
typedef uint64_t BrDeadline;

#define BR_TIMEOUT_INFINITY ((BrTimeout)-1)
typedef uint64_t BrTimeout;

// Events

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
        int irq;
    };
} BrEvent;

// Messages

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
#define BR_MSG_ARG_COUNT (5)
        BrArg args[BR_MSG_ARG_COUNT];
        BrEvent event;
    };
} BrMsg;

_Static_assert(sizeof(BrMsg) == 64, "");

// Rights

#define BR_RIGHT_IRQ (1 << 0)
#define BR_RIGHT_PMM (1 << 1)
#define BR_RIGHT_LOG (1 << 2)
#define BR_RIGHT_TASK (1 << 3)
#define BR_RIGHT_IO (1 << 4)

typedef uint32_t BrRight;
