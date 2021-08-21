#pragma once

#include <brutal/base.h>
#include <stdbool.h>
#include <stdint.h>
#include "brutal/base/attributes.h"

#define FIBER_STACK_SIZE 4096

#define FIBER_FXSAVE_REGION_SIZE 750

#define FIBER_CURRENT_RUNNING_PROC (nullptr)

typedef void fiber_function(void);

typedef struct PACKED
{
    uintptr_t rip, rsp;
    uintptr_t rbx, rbp, r12, r13, r14, r15;
    uint32_t fc_mxcsr;
    uint32_t fc_x86_cw;
} FibersContext;

enum fiber_state
{
    FIBER_RUNNING,
    FIBER_MUST_BE_DELETED,
    FIBER_SUSPENDED,
    FIBER_ERROR,
};

typedef bool fiber_blocker_func(void *context);

typedef struct
{
    void *context;
    fiber_blocker_func *function;
} FiberBlocker;

static inline FiberBlocker fiber_block(fiber_blocker_func *func, void *ctx)
{
    return (FiberBlocker){
        .function = func,
        .context = ctx};
}

typedef struct fiber
{
    struct fiber *next;
    void *stack;

    enum fiber_state state;
    FiberBlocker block;
    int uid;

    FibersContext regs;
} Fiber;

void fiber_yield(void);

// use FIBER_CURRENT_RUNNING_PROC for creating current running code as a fiber, note: rsp/rip are not loaded and are saved by fiber_switch
Fiber *fiber_start(fiber_function call);

void fiber_pause(Fiber *fiber, FiberBlocker blocker);

void fiber_stop(Fiber *fiber);

Fiber *fiber_self(void);
