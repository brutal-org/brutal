#pragma once

#include <brutal/base.h>
#include <brutal/text.h>
#include <brutal/types.h>
#include "arch/heap.h"
#include "kernel/channel.h"
#include "kernel/domain.h"
#include "kernel/memory.h"

#define CPU_SCHEDULER_MANAGER 0

/* --- Task object ---------------------------------------------------------- */

typedef enum
{
    TASK_NONE = 0,
    TASK_USER = 1 << 0,
} TaskFlags;

typedef enum
{
    TASK_STATE_NONE,
    TASK_STATE_IDLE,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
} TaskState;

// See scheduler.md in the book for more information
typedef struct
{
    CpuId cpu; // Only valid if tick_in_cpu is >= 0
    int tick_start;
    int tick_end;
    bool is_currently_executed;
} TaskSchedule;

typedef struct
{
    Object base;

    StrFix128 name;
    TaskFlags flags;
    TaskState state;

    TaskSchedule schedule;

    BrCap caps;
    Space *space;
    Domain *domain;
    Channel *channel;

    uintptr_t sp;
    HeapRange stack;
} Task;

typedef Result(BrResult, Task *) TaskCreateResult;

Task *task_self(void);

TaskCreateResult task_create(Str name, Space *space, TaskFlags flags);

void task_ref(Task *self);

void task_deref(Task *self);

void task_start(Task *self, uintptr_t ip, uintptr_t sp, BrTaskArgs args);

void task_state(Task *self, TaskState state);

/* --- Task Management ------------------------------------------------------ */

void tasking_initialize(void);

/* --- Task Scheduling ------------------------------------------------------ */

struct schedule
{
    Task *idle;
    Task *current;
    Task *next;
};

void scheduler_switch(void);

void scheduler_schedule_and_switch(void);
