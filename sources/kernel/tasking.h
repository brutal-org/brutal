#pragma once

#include <brutal/base.h>
#include <brutal/text.h>
#include <brutal/types.h>
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
    uintptr_t base;
    uintptr_t size;
} Stack;

typedef struct
{
    TaskId id;
    StrFix128 name;
    TaskFlags flags;
    TaskState state;

    TaskSchedule schedule;

    MemorySpace *space;

    uintptr_t ksp;
    Stack kernel_stack;

    uintptr_t usp;
    Stack user_stack;
} Task;

typedef Result(BrResult, Task *) TaskCreateResult;

Task *task_self(void);

TaskCreateResult task_create(Str name, TaskFlags flags);

void task_start(Task *self, uintptr_t ip, uintptr_t sp, BrTaskArgs args);

void task_state(Task *self, TaskState state);

void task_wait(Task *self, uint64_t ms);

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
