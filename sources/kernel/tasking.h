#pragma once

#include <brutal/base.h>
#include <brutal/types.h>
#include "syscalls/error.h"

#define CPU_SCHEDULER_MANAGER 0

/* --- Task object ---------------------------------------------------------- */

enum task_state
{
    TASK_NONE,
    TASK_IDLE,
    TASK_RUNNING,
    TASK_WAITING,
    TASK_SLEEPING
};

enum task_level
{
    TASK_LEVEL_USER,
    TASK_LEVEL_LOW,
    TASK_LEVEL_KERNEL,
};

// see scheduler.md in the book for more information
struct task_schedule_state
{
    cpu_id_t task_cpu; // only valid if tick_in_cpu is >= 0
    int tick_start;
    int tick_end;
    bool is_currently_executed;
};

struct task
{
    task_id_t id;
    enum task_state state;

    int level;

    struct task_schedule_state scheduler_state;

    uintptr_t ip;
    uintptr_t sp;
    uintptr_t kernel_sp;
};

typedef result_t(br_error_t, struct task *) task_return_result_t;

struct task *task_self(void);

task_return_result_t task_spawn(uintptr_t ip, bool start_direct);

void task_go(struct task *self);

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

uintptr_t tasking_switch(uintptr_t sp);

uintptr_t tasking_schedule_and_switch(uintptr_t sp);
