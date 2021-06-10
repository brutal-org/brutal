#include <brutal/ds.h>
#include <brutal/log.h>
#include <brutal/sync.h>
#include "arch/cpu.h"
#include "arch/heap.h"
#include "arch/task.h"
#include "kernel/constants.h"
#include "kernel/tasking.h"

static struct lock task_lock = {};
static int task_id = 0;
static vec_t(struct task *) tasks = {};

struct task *task_self(void)
{
    return cpu_self()->schedule.current;
}

task_return_result_t task_spawn(uintptr_t ip)
{
    LOCK_RETAINER(&task_lock);

    auto task = TRY(task_return_result_t, arch_task_create(ip, 0, 0, 0, 0));

    task->id = task_id++;
    task->ip = ip;
    task->level = TASK_LEVEL_USER; // not used for the moment

    vec_push(&tasks, task);

    log("Task({}) created...", task->id);

    return OK(task_return_result_t, task);
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
    if (cpu_self()->schedule.next == NULL)
    {
        cpu_self()->schedule.current = cpu_self()->schedule.idle;
        return;
    }
    else
    {
        cpu_self()->schedule.current = cpu_self()->schedule.next;
        cpu_self()->schedule.next = NULL;
    }
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

    task_self()->sp = sp; // save stack pointer
    arch_task_save_context(cpu_self()->schedule.current);

    tasking_schedule();

    sp = task_self()->sp; // load stack pointer

    arch_task_load_context(cpu_self()->schedule.current);

    return tasking_switch(sp);
}
