#pragma once

#include <bal/abi.h>
#include <brutal-text>
#include <brutal-time>

#include "kernel/channel.h"
#include "kernel/context.h"
#include "kernel/space.h"

typedef bool BlockerFn(void *context);

typedef struct
{
    BlockerFn *function;
    void *context;

    Tick deadline;
    BrResult result;
} Blocker;

typedef struct
{
    OBJECT_HEADER;

    BrTaskFlags flags;

    Blocker blocker;

    bool in_syscall;
    bool stopped;
    bool blocked;
    bool started;

    size_t time_start;
    size_t time_end;

    BrRight rights;
    Context *context;
    Space *space;
    Domain *domain;
    Channel *channel;

    uintptr_t sp;
    HeapRange stack;

    uintptr_t result;
} Task;

typedef Result(BrResult, Task *) TaskCreateResult;

Task *task_self(void);

TaskCreateResult task_create(Space *space, BrRight rights, BrTaskFlags flags);

void task_ref(Task *self);

void task_deref(Task *self);

void task_begin_syscall(void);

void task_end_syscall(void);

static inline bool task_runnable(Task *self)
{
    bool blocked_or_stopped = self->blocked || (self->stopped && !self->in_syscall);
    return self->started && (!blocked_or_stopped);
}
