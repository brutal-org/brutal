#pragma once

#include <library/base.h>

/* --- Task object ---------------------------------------------------------- */

enum task_state
{
    TASK_NONE,
    TASK_IDLE,
    TASK_RUNNING,
};

struct task
{
    int id;
    enum task_state state;

    uintptr_t ip;
    uintptr_t sp;
};

struct task *task_self(void);

struct task *task_spawn(uintptr_t ip);

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
