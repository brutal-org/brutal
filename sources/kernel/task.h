#pragma once

#include <brutal/text.h>
#include <brutal/time.h>
#include <brutal/types.h>
#include <syscalls/types.h>
#include "kernel/channel.h"
#include "kernel/context.h"
#include "kernel/space.h"

typedef enum
{
    BLOCKER_NONE,

    BLOCKER_SEND,
    BLOCKER_RECV,
    BLOCKER_TIME,
} BlockerType;

typedef struct
{
    BlockerType type;
    Tick deadline;
    BrResult result;
} Blocker;

typedef struct
{
    OBJECT_HEADER;

    StrFix128 name;
    BrTaskFlags flags;

    Blocker blocker;

    CpuId cpu;

    bool in_syscall;
    bool is_stopped;
    bool is_blocked;
    bool is_started;

    size_t time_start;
    size_t time_end;

    BrCap caps;
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

TaskCreateResult task_create(Str name, Space *space, BrCap caps, BrTaskFlags flags);

void task_ref(Task *self);

void task_deref(Task *self);

void task_begin_syscall(void);

void task_end_syscall(void);

static inline bool task_runnable(Task *self)
{
    bool blocked_or_stopped = (self->is_blocked || self->is_stopped) && !self->in_syscall;
    return self->is_started && !blocked_or_stopped;
}

static inline bool task_running(Task *self)
{
    return self->cpu != CPU_NONE;
}
