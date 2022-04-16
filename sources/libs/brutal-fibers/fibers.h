#pragma once

#include <brutal-mem>
#include <brutal-time>

#define FIBER_STACK_SIZE KiB(16)

#define FIBER_CURRENT_RUNNING_PROC (nullptr)

typedef void *FiberFn(void *arg);

typedef struct PACKED
{
    uint64_t rip, rsp;
    uint64_t rbx, rbp, r12, r13, r14, r15;

    uint32_t fc_mxcsr;
    uint32_t fc_x86_cw;
} FibersContext;

typedef enum
{
    FIBER_EMBRYO,
    FIBER_RUNNING,
    FIBER_CANCELING,
    FIBER_CANCELED,
    FIBER_BLOCKED,
    FIBER_IDLE,

    FIBER_ERROR,
} FiberState;

typedef bool FiberBlockerFn(void *context);

typedef enum
{
    FIBER_SUCCESS,
    FIBER_TIMEOUT,
    FIBER_INTERRUPTED,
} FiberBlockResult;

typedef struct
{
    FiberBlockerFn *function;
    void *context;
    Timeout deadline;

    FiberBlockResult result;
} FiberBlocker;

typedef struct _Fiber Fiber;

struct _Fiber
{
    int id;
    FiberState state;
    FiberBlocker blocker;
    bool fire_and_forget;

    FiberFn *fn;
    void *args;
    void *ret;

    void *stack;
    FibersContext ctx;

    Fiber *next;
    Fiber *prev;
};

void fiber_yield(void);

Fiber *fiber_start(FiberFn fn, void *args);

void fiber_start_and_forget(FiberFn fn, void *args);

FiberBlockResult fiber_block(FiberBlocker blocker);

void fiber_sleep(Timeout timeout);

void fiber_ret(void *ret);

void *fiber_await(Fiber *fiber);

Fiber *fiber_self(void);

Tick fiber_deadline(void);
