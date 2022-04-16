#pragma once

#include <brutal-text>

#define TASK_THIS (0)

#define TASK_EXIT_SUCCESS (0)
#define TASK_EXIT_FAILURE (-1)

enum _TaskType
{
    TASK_FORK,
    TASK_CLONE,
    TASK_EXEC,
};

struct _TaskFork
{
    int _dummy;
};

typedef void task_clone_entry_t(void);

struct _TaskClone
{
    task_clone_entry_t *entry;
};

struct _TaskExec
{
    int _dummy;
};

struct _Task
{
    int handle;
    enum _TaskType type;

    union
    {
        struct _TaskFork fork;
        struct _TaskClone clone;
        struct _TaskExec exec;
    };
};

struct _Task *task_self(void);

void task_fork(struct _Task *task);

void task_fork_func(struct _Task *task, task_clone_entry_t *entry);

void task_clone(struct _Task *task);

void task_clone_func(struct _Task *task, task_clone_entry_t *entry);

void task_exec(struct _Task *task);

bool task_is_child(struct _Task *task);

void task_run(struct _Task *task);

void task_exit(struct _Task *task, int result);

void task_abort(struct _Task *task);

typedef Result(Error, int) TaskWaitResult;

TaskWaitResult task_wait(struct _Task *task);
