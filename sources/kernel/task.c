#include <library/ds/vec.h>
#include <library/log.h>
#include <library/task/lock.h>
#include "arch/heap.h"
#include "kernel/constants.h"
#include "kernel/scheduler.h"
#include "kernel/task.h"

static struct lock task_lock;
static int task_id;
static vec_t(struct task *) tasks;

void tasking_initialize(void)
{
    log("Initializing tasking...");

    vec_init(&tasks, alloc_global());
}

struct task *task_self(void)
{
    return nullptr;
}

void task_run(uintptr_t ip)
{
    LOCK_RETAINER(&task_lock);

    auto task = alloc_make(alloc_global(), struct task);

    task->id = task_id++;
    task->ip = ip;
    task->sp = UNWRAP(heap_alloc(KERNEL_STACK_SIZE)).base + KERNEL_STACK_SIZE;

    vec_push(&tasks, task);
}

void task_state(struct task *self, enum task_state new_state)
{
    LOCK_RETAINER(&task_lock);

    if (self->state == new_state)
    {
        return;
    }

    scheduler_update(self, self->state, new_state);
    self->state = new_state;
}

void task_wait(struct task *self, uint64_t ms)
{
    UNUSED(self);
    UNUSED(ms);

    // TODO: task wait
}