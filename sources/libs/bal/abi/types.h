#pragma once

#include <brutal-base/range.h>
#include <brutal-time>

/* --- Syscalls IDs --------------------------------------------------------- */

#define FOREACH_SYSCALLS(SYSCALL) \
    SYSCALL(LOG)                  \
    SYSCALL(NOW)                  \
    SYSCALL(MAP)                  \
    SYSCALL(UNMAP)                \
    SYSCALL(CREATE)               \
    SYSCALL(DUP)                  \
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

#define BR_MEM_READABLE (1 << 0)
#define BR_MEM_WRITABLE (1 << 1)
#define BR_MEM_EXECUTABLE (1 << 2)
#define BR_MEM_USER (1 << 3)
#define BR_MEM_LOWER (1 << 4)
#define BR_MEM_DMA (1 << 5)
#define BR_MEM_GLOBAL (1 << 6)
#define BR_MEM_WRITE_THROUGHT (1 << 7)

typedef uint64_t BrMemoryFlags;

// Domain Objects

typedef uint64_t BrDomainFlags;

// Space Objects

typedef uint64_t BrSpaceFlags;

/// Task Objects

#define BR_TASK_USER (BrTaskFlags)(1 << 0)

typedef uint64_t BrTaskFlags;

typedef struct
{
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

typedef uint64_t BrTick;

typedef uint64_t BrTimeStamp;

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
    BrId id;
#define BR_PORT_NIL ((uint64_t)0)
#define BR_PORT_ANY ((uint64_t)-1)
    uint64_t port;
} BrAddr;

#define BR_ADDR_NIL \
    (BrAddr) { BR_ID_NIL, BR_PORT_ANY }

#define BR_ADDR_ERR \
    (BrAddr) { BR_ID_ERR, BR_PORT_ANY }

#define BR_ADDR_EVENT \
    (BrAddr) { BR_ID_EVENT, BR_PORT_ANY }

#define BR_ADDR_SELF \
    (BrAddr) { BR_ID_SELF, BR_PORT_ANY }

typedef struct
{
    BrMsgFlags flags;

    BrAddr from;
    BrAddr to;

#define BR_MSG_ERROR (uint32_t)(-1)
    uint64_t prot;
    uint64_t type;
    uint64_t seq;

    union
    {
#define BR_MSG_ARG_COUNT (5)
        BrArg args[BR_MSG_ARG_COUNT];
        BrEvent event;
    };
} BrMsg;

// Rights

#define BR_RIGHT_IRQ (1 << 0)
#define BR_RIGHT_DMA (1 << 1)
#define BR_RIGHT_PIO (1 << 2)
#define BR_RIGHT_LOG (1 << 3)
#define BR_RIGHT_TASK (1 << 4)

typedef uint32_t BrRight;

static inline BrRight br_right_from_str(Str str)
{
    if (str_eq(str, str$("irq")))
        return BR_RIGHT_IRQ;
    else if (str_eq(str, str$("dma")))
        return BR_RIGHT_DMA;
    else if (str_eq(str, str$("pio")))
        return BR_RIGHT_PIO;
    else if (str_eq(str, str$("log")))
        return BR_RIGHT_LOG;
    else if (str_eq(str, str$("task")))
        return BR_RIGHT_TASK;
    else
        return 0;
}
