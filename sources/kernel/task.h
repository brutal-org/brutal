#pragma once

#include <library/text/str.h>

enum task_state
{
    TASK_NONE,
    TASK_RUNNING,
};

struct task
{
    int id;
    enum task_state state;

    uintptr_t ip;
    uintptr_t sp;
};

void tasking_initialize(void);

struct task *task_self(void);

void task_run(uintptr_t ip);

void task_state(struct task *self, enum task_state state);

void task_wait(struct task *self, uint64_t ms);
