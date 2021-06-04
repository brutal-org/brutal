#pragma once

#include <library/base/error.h>
#include <library/base/str.h>

#define TASK_THIS (-424242)

#define TASK_EXIT_SUCCESS (0)
#define TASK_EXIT_FAILURE (-1)

typedef unsigned int task_handle_t;
typedef int task_result_t;

enum task_type
{
    TASK_FORK,
    TASK_CLONE,
    TASK_EXEC,
};

struct task_fork
{
};

typedef void task_clone_entry_t(void);

struct task_clone
{
    task_clone_entry_t *entry;
};

struct task_exec
{
};

struct task
{
    task_handle_t handle;
    enum task_type type;

    union
    {
        struct task_fork fork;
        struct task_clone clone;
        struct task_exec exec;
    };
};

struct task *task_this(void);

void task_fork(struct task *task);

void task_fork_func(struct task *task, task_clone_entry_t *entry);

void task_clone(struct task *task);

void task_clone_func(struct task *task, task_clone_entry_t *entry);

void task_exec(struct task *task);

bool task_child(struct task *task);

void task_run(struct task *task);

void task_exit(struct task *task, int result);

void task_abort(struct task *task);

struct task_wait_result
{
    struct error error;
    task_result_t result;
};

struct task_wait_result task_wait(struct task *task);
