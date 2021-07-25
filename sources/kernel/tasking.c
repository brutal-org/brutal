#include <brutal/alloc.h>
#include <brutal/ds.h>
#include <brutal/log.h>
#include <brutal/sync.h>
#include "kernel/arch.h"
#include "kernel/cpu.h"
#include "kernel/heap.h"
#include "kernel/kernel.h"
#include "kernel/tasking.h"

static Lock task_lock = {};
static Vec(Task *) tasks = {};
static atomic_uint_least64_t tick = 0;

Task *task_self(void)
{
    return cpu_self()->current;
}

void task_destroy(Task *task)
{
    log("Destroying task({}) {}", task->base.handle, str_cast(&task->name));

    context_destroy(task->context);
    space_deref(task->space);
    domain_deref(task->domain);
    channel_destroy(task->channel);
    heap_free(task->stack);

    alloc_free(alloc_global(), task);
}

TaskCreateResult task_create(Str name, Space *space, BrCap caps, BrTaskFlags flags)
{
    log("Creating Task({})...", name);

    auto task = alloc_make(alloc_global(), Task);

    task->name = str_cast_fix(StrFix128, name);
    task->flags = flags;

    task->caps = caps;
    task->context = context_create();
    space_ref(space);
    task->space = space;
    task->domain = domain_create();
    task->channel = channel_create();

    // Create the kernel stack
    task->stack = UNWRAP(heap_alloc(KERNEL_STACK_SIZE));
    task->sp = range_end(task->stack);

    object_init(base_cast(task), OBJECT_TASK, (ObjectDtor *)task_destroy);

    log("Task {}({}) created...", str_cast(&task->name), task->base.handle);

    return OK(TaskCreateResult, task);
}

void task_ref(Task *self)
{
    object_ref(base_cast(self));
}

void task_deref(Task *self)
{
    object_deref(base_cast(self));
}

void task_start(Task *self, uintptr_t ip, uintptr_t sp, BrTaskArgs args)
{
    LOCK_RETAINER(&task_lock);

    assert_truth(self->state == TASK_STATE_NONE);

    context_start(self->context, ip, sp, args, self->flags);
    self->state = TASK_STATE_RUNNING;

    task_ref(self);
    vec_push(&tasks, self);
}

void task_cancel(Task *self, uintptr_t result)
{
    log("Canceling task {}...", str_cast(&self->name));

    lock_acquire(&task_lock);

    if (self->is_canceled)
    {
        lock_release(&task_lock);
        return;
    }

    self->is_canceled = true;
    self->result = result;

    lock_release(&task_lock);

    log("Task marked as canceled, killing it...");

    task_cancel_if_needed(self);
}

void task_cancel_if_needed(Task *self)
{
    lock_acquire(&task_lock);

    if (self->in_syscall)
    {
        lock_release(&task_lock);
        return;
    }

    if (!self->is_canceled)
    {
        lock_release(&task_lock);
        return;
    }

    self->state = TASK_STATE_CANCELING;

    lock_release(&task_lock);

    log("Task canceled, yielding to other task now.");

    if (self == task_self())
    {
        scheduler_yield();
        assert_unreachable();
    }
}

void task_begin_syscall(Task *self)
{
    assert_truth(task_self() == self);

    lock_acquire(&task_lock);

    if (self->is_canceled)
    {
        lock_release(&task_lock);

        scheduler_yield();
        assert_unreachable();
    }
    else
    {

        self->in_syscall = true;
        lock_release(&task_lock);
    }
}

void task_end_syscall(Task *self)
{
    assert_truth(task_self() == self);

    lock_acquire(&task_lock);
    self->in_syscall = false;
    lock_release(&task_lock);

    task_cancel_if_needed(self);
}

void task_block(Task *self, Blocker blocker)
{
    lock_acquire(&task_lock);

    self->blocker = blocker;
    self->state = TASK_STATE_BLOCKED;

    lock_release(&task_lock);

    scheduler_yield();
}

void task_sleep(Task *self, BrTimeout timeout)
{
    task_block(self, (Blocker){.type = BLOCKER_TIME, .deadline = tick + timeout});
}

void idle(void)
{
    while (true)
        arch_idle();
}

Task *tasking_create_idle(void)
{
    auto space = space_create(BR_SPACE_NONE);
    auto task = UNWRAP(task_create(str_cast("idle"), space, BR_CAP_NONE, BR_TASK_NONE));

    context_start(task->context, (uintptr_t)idle, task->sp, (BrTaskArgs){}, task->flags);
    task->state = TASK_STATE_IDLE;

    return task;
}

Task *tasking_create_boot(void)
{
    auto space = space_create(BR_SPACE_NONE);
    auto task = UNWRAP(task_create(str_cast("boot"), space, BR_CAP_NONE, BR_TASK_NONE));
    space_deref(space);
    task_start(task, 0, 0, (BrTaskArgs){});
    task->is_running = true;

    task_deref(task);

    return task;
}

static inline void finalizer(void)
{
    while (true)
    {
        log("Finalizing tasks...");

        lock_acquire(&task_lock);

        for (int i = 0; i < tasks.length; i++)
        {
            auto task = tasks.data[i];

            if (task->state == TASK_STATE_CANCELED)
            {
                log("Finalizing task({}) {}", task->base.handle, str_cast(&task->name));
                vec_remove(&tasks, task);
                task_deref(task);
                i--;
            }
        }

        lock_release(&task_lock);

        task_sleep(task_self(), 250);
    }
}

void tasking_create_finalizer(void)
{
    auto space = space_create(BR_SPACE_NONE);
    auto task = UNWRAP(task_create(str_cast("finalizer"), space, BR_CAP_NONE, BR_TASK_NONE));
    space_deref(space);
    task_start(task, (uintptr_t)finalizer, task->sp, (BrTaskArgs){});
    task_deref(task);
}

void tasking_initialize(void)
{
    log("Initializing tasking...");

    vec_init(&tasks, alloc_global());

    for (size_t i = 0; i < cpu_count(); i++)
    {
        log("Initializing tasking for cpu: {}", i);
        cpu(i)->idle = tasking_create_idle();
        cpu(i)->current = tasking_create_boot();
        cpu(i)->next = cpu(i)->idle;
    }

    tasking_create_finalizer();
}

void scheduler_yield(void)
{
    task_self()->yield = true;

    while (task_self()->yield)
    {
        arch_idle();
    }
}

void scheduler_unblock(void)
{
    vec_foreach(task, &tasks)
    {
        if (task->state == TASK_STATE_BLOCKED)
        {
            if (task->blocker.deadline < tick)
            {
                task->state = TASK_STATE_RUNNING;
            }
        }
    }
}

void scheduler_update(void)
{
    for (size_t i = 0; i < cpu_count(); i++)
    {
        Task *current = cpu(i)->current;
        Task *next = nullptr;

        current->is_running = false;

        vec_foreach(task, &tasks)
        {
            if (task->state == TASK_STATE_RUNNING &&
                !task->is_running &&
                (next == nullptr || task->last_tick < next->last_tick))
            {
                next = task;
            }
        }

        if (next == nullptr)
        {
            next = cpu(i)->idle;
        }

        next->last_tick = tick;
        next->is_running = true;

        cpu(i)->next = next;
    }
}

void scheduler_switch_other(void)
{
    for (size_t i = 1; i < cpu_count(); i++)
    {
        if (cpu(i)->current != cpu(i)->next)
        {
            cpu_resched_other(i);
        }
    }
}

void scheduler_switch(void)
{
    if (task_self()->state == TASK_STATE_CANCELING)
    {
        task_self()->state = TASK_STATE_CANCELED;
    }

    cpu_self()->current->yield = false;
    cpu_self()->current = cpu_self()->next;
}

void scheduler_schedule_and_switch(void)
{
    tick++;

    if (!lock_try_acquire(&task_lock))
    {
        return;
    }

    scheduler_unblock();
    scheduler_update();
    scheduler_switch_other();

    if (cpu_self()->current != cpu_self()->next)
    {
        scheduler_switch();
    }

    lock_release(&task_lock);
}
