#include <library/alloc/global.h>
#include <library/ds/vec.h>
#include <library/log.h>
#include <library/task/lock.h>
#include "kernel/scheduler.h"

static struct lock scheduler_lock;
static vec_t(struct task *) running_tasks;

void scheduler_initialize(void)
{
    log("Initializing scheduler...");

    vec_init(&running_tasks, alloc_global());
}

void scheduler_update(struct task *task, enum task_state old_state, enum task_state new_state)
{
    LOCK_RETAINER(&scheduler_lock);

    if (old_state == TASK_NONE && new_state == TASK_RUNNING)
    {
        vec_push(&running_tasks, task);
    }

    if (old_state == TASK_RUNNING && new_state == TASK_NONE)
    {
        vec_remove(&running_tasks, task);
    }
}

uintptr_t scheduler_schedule(uintptr_t sp)
{
    return sp;
}
