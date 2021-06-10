#pragma once

#include <brutal/base.h>
#include "syscalls/error.h"

/* --- Task object ---------------------------------------------------------- */

enum task_state
{
    TASK_NONE,
    TASK_IDLE,
    TASK_RUNNING,
};

enum task_level
{
    TASK_LEVEL_USER = 0,
    TASK_LEVEL_MODULE = 1,
    TASK_LEVEL_KERNEL = 2,
};

struct task
{
    int id;
    enum task_state state;

    int level;

    uintptr_t ip;
    uintptr_t sp;
    uintptr_t kernel_sp;
};

typedef result_t(br_error_t, struct task *) task_return_result_t;

struct task *task_self(void);

task_return_result_t task_spawn(uintptr_t ip);

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

void tasking_schedule(void);

uintptr_t tasking_switch(uintptr_t sp);

uintptr_t tasking_schedule_and_switch(uintptr_t sp);
