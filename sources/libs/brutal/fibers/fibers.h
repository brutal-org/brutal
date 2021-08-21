#pragma once

#include <brutal/base.h>
#include <brutal/time.h>

#define FIBER_STACK_SIZE KiB(16)

#define FIBER_CURRENT_RUNNING_PROC (nullptr)

typedef void FiberFn(void);

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

    FIBER_ERROR,
} FiberState;

typedef bool FiberBlockerFn(void *context);

typedef struct
{
    FiberBlockerFn *function;
    void *context;
    Tick deadline;
} FiberBlocker;

typedef struct fiber Fiber;

struct fiber
{
    int id;
    FiberState state;
    FiberBlocker blocker;

    FiberFn *fn;

    void *stack;
    FibersContext ctx;

    Fiber *next;
    Fiber *prev;
};

void fiber_yield(void);

Fiber *fiber_start(FiberFn call);

void fiber_pause(FiberBlocker blocker);

void fiber_ret(void);

void fiber_await(Fiber *fiber);

Fiber *fiber_self(void);
