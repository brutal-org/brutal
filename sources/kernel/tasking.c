#include <brutal/ds/vec.h>
#include <brutal/log.h>
#include <brutal/task/lock.h>
#include "arch/heap.h"
#include "kernel/constants.h"
#include "kernel/tasking.h"

static struct lock task_lock = {};
static int task_id = 0;
static vec_t(struct task *) tasks = {};

struct task *task_self(void)
{
    return nullptr;
}

struct task *task_spawn(uintptr_t ip)
{
    LOCK_RETAINER(&task_lock);

    auto task = alloc_make(alloc_global(), struct task);

    task->id = task_id++;
    task->ip = ip;
    task->sp = UNWRAP(heap_alloc(KERNEL_STACK_SIZE)).base + KERNEL_STACK_SIZE;

    vec_push(&tasks, task);

    log("Task({}) created...", task->id);

    return task;
}

void task_state(struct task *self, enum task_state new_state)
{
    LOCK_RETAINER(&task_lock);

    if (self->state == new_state)
    {
        return;
    }

    self->state = new_state;
}

void task_wait(struct task *self, uint64_t ms)
{
    UNUSED(self);
    UNUSED(ms);

    // TODO: task wait
}

void tasking_initialize(void)
{
    log("Initializing tasking...");

    LOCK_RETAINER(&task_lock);
    vec_init(&tasks, alloc_global());
}

void tasking_schedule(void)
{
}

uintptr_t tasking_switch(uintptr_t sp)
{
    return sp;
}

uintptr_t tasking_schedule_and_switch(uintptr_t sp)
{
    if (!lock_try_acquire(&task_lock))
    {
        return sp;
    }

    tasking_schedule();
    return tasking_switch(sp);
}
