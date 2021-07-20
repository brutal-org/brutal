#pragma once

#include <brutal/base.h>
#include <brutal/text.h>
#include <brutal/time.h>
#include <brutal/types.h>
#include "kernel/channel.h"
#include "kernel/context.h"
#include "kernel/domain.h"
#include "kernel/heap.h"
#include "kernel/memory.h"

#define CPU_SCHEDULER_MANAGER 0

/* --- Task object ---------------------------------------------------------- */

typedef enum
{
    BLOCKER_NONE,

    BLOCKER_IPC,
    BLOCKER_TIME,
} BlockerType;

typedef struct
{
    BlockerType type;
    Tick deadline;
} Blocker;
typedef enum
{
    TASK_STATE_NONE,

    TASK_STATE_IDLE,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
    TASK_STATE_CANCELING,
    TASK_STATE_CANCELED,
} TaskState;

typedef struct
{
    Object base;

    StrFix128 name;
    BrTaskFlags flags;

    TaskState state;
    Blocker blocker;

    bool in_syscall;
    bool is_canceled;
    bool is_running;
    size_t last_tick;
    atomic_bool yield;

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

TaskCreateResult task_create(Str name, Space *space, BrTaskFlags flags);

void task_ref(Task *self);

void task_deref(Task *self);

void task_start(Task *self, uintptr_t ip, uintptr_t sp, BrTaskArgs args);

void task_cancel(Task *self, uintptr_t result);

void task_cancel_if_needed(Task *self);

void task_begin_syscall(Task *self);

void task_end_syscall(Task *self);

void task_block(Task *self, Blocker blocker);

void task_sleep(Task *self, BrTimeout timeout);

/* --- Task Management ------------------------------------------------------ */

void tasking_initialize(void);

/* --- Task Scheduling ------------------------------------------------------ */

void scheduler_yield(void);

void scheduler_switch(void);

void scheduler_schedule_and_switch(void);
