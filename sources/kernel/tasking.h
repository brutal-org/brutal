#pragma once

#include <arch/vmm.h>
#include <brutal/base.h>
#include <brutal/text.h>
#include <brutal/types.h>
#include "syscalls/error.h"

#define CPU_SCHEDULER_MANAGER 0

/* --- Task object ---------------------------------------------------------- */

enum task_state
{
    TASK_STATE_NONE,
    TASK_STATE_IDLE,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
};

enum task_flags
{
    TASK_NONE = 0,
    TASK_USER = 1 << 0,
};

// see scheduler.md in the book for more information
struct task_schedule_state
{
    CpuId cpu; // only valid if tick_in_cpu is >= 0
    int tick_start;
    int tick_end;
    bool is_currently_executed;
};

struct stack
{
    uintptr_t base;
    uintptr_t size;
};

struct task
{
    TaskId id;
    StrFix128 name;
    enum task_flags flags;
    enum task_state state;

    vmm_space_t virtual_memory_space;

    struct task_schedule_state scheduler_state;

    uintptr_t sp;
    struct stack kernel_stack;
};


typedef Result(int, struct task *) task_return_Result;

struct task *task_self(void);

task_return_Result task_create(Str name, enum task_flags flags);

void task_start(
    struct task *self,
    uintptr_t ip,
    uintptr_t arg1,
    uintptr_t arg2,
    uintptr_t arg3,
    uintptr_t arg4,
    uintptr_t arg5);

void task_state(struct task *self, enum task_state state);

void task_wait(struct task *self, uint64_t ms);

/* --- Task Management ------------------------------------------------------ */

void tasking_initialize(void);

/* --- Task Scheduling ------------------------------------------------------ */

struct schedule
{
    struct task *idle;
    struct task *current;
    struct task *next;
};

uintptr_t scheduler_switch(uintptr_t sp);

uintptr_t scheduler_schedule_and_switch(uintptr_t sp);
