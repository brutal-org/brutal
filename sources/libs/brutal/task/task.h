#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/text/str.h>
#include <brutal/types.h>

#define TASK_THIS (0)

#define TASK_EXIT_SUCCESS (0)
#define TASK_EXIT_FAILURE (-1)

enum task_type
{
    TASK_FORK,
    TASK_CLONE,
    TASK_EXEC,
};

struct task_fork
{
    int _dummy;
};

typedef void task_clone_entry_t(void);

struct task_clone
{
    task_clone_entry_t *entry;
};

struct task_exec
{
    int _dummy;
};

struct task
{
    TaskId handle;
    enum task_type type;

    union
    {
        struct task_fork fork;
        struct task_clone clone;
        struct task_exec exec;
    };
};

struct task *task_self(void);

void task_fork(struct task *task);

void task_fork_func(struct task *task, task_clone_entry_t *entry);

void task_clone(struct task *task);

void task_clone_func(struct task *task, task_clone_entry_t *entry);

void task_exec(struct task *task);

bool task_child(struct task *task);

void task_run(struct task *task);

void task_exit(struct task *task, int result);

void task_abort(struct task *task);

typedef Result(Error, int) TaskWaitResult;
TaskWaitResult task_wait(struct task *task);
