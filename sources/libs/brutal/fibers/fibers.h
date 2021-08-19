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
    uintptr_t fxsave[FIBER_FXSAVE_REGION_SIZE];
} fibers_reg;

enum fiber_state
{
    FIBER_RUNNING,
    FIBER_MUST_BE_DELETED,
    FIBER_SUSPENDED,
    FIBER_ERROR,
};

typedef struct fiber
{
    struct fiber *next;
    void *stack;

    enum fiber_state state;
    int uid;

    fibers_reg regs ALIGNED(64);
} Fiber;

void fiber_switch();

// use FIBER_CURRENT_RUNNING_PROC for creating current running code as a fiber, note: rsp/rip are not loaded and are saved by fiber_switch
Fiber *fiber_init(fiber_function call);

void pause_fiber(Fiber *fiber);

void stop_fiber(Fiber *fiber);

Fiber *fiber_self();
