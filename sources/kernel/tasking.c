#include <brutal/alloc.h>
#include <brutal/ds.h>
#include <brutal/log.h>
#include <brutal/sync.h>
#include "kernel/cpu.h"
#include "kernel/heap.h"
#include "kernel/kernel.h"
#include "kernel/tasking.h"

static Lock task_lock = {};
static Vec(Task *) tasks = {};
size_t current_tick = 0;

void task_idle(void)
{
    while (true)
    {
        asm volatile("hlt");
    }
}

Task *task_self(void)
{
    return cpu_self()->schedule.current;
}

void task_destroy(Task *task)
{
    context_destroy(task->context);
    space_deref(task->space);
    domain_deref(task->domain);
    channel_destroy(task->channel);
    heap_free(task->stack);

    alloc_free(alloc_global(), task);
}

TaskCreateResult task_create(Str name, Space *space, BrTaskFlags flags)
{
    log("Creating Task({})...", name);

    auto task = alloc_make(alloc_global(), Task);

    task->name = str_cast_fix(StrFix128, name);
    task->flags = flags;

    task->context = context_create();
    space_ref(space);
    task->space = space;
    task->domain = domain_create();
    task->channel = channel_create();

    // Create the kernel stack
    task->stack = UNWRAP(heap_alloc(KERNEL_STACK_SIZE));
    task->sp = range_end(task->stack);

    log("Task({}) created...", str_cast(&task->name));

    object_init(base_cast(task), OBJECT_TASK, (ObjectDtor *)task_destroy);

    lock_acquire(&task_lock);
    vec_push(&tasks, task);
    lock_release(&task_lock);

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

    context_start(self->context, ip, sp, args, self->flags);
    self->state = TASK_STATE_RUNNING;
}

Task *task_create_idle(void)
{
    auto space = space_create();
    auto task = UNWRAP(task_create(str_cast("idle"), space, BR_TASK_NONE));
    context_start(task->context, (uintptr_t)task_idle, task->sp, (BrTaskArgs){}, task->flags);
    task->state = TASK_STATE_IDLE;
    return task;
}

Task *task_create_boot(void)
{
    auto space = space_create();
    auto task = UNWRAP(task_create(str_cast("boot"), space, BR_TASK_NONE));
    space_deref(space);
    task_start(task, 0, 0, (BrTaskArgs){});
    return task;
}

void task_state(Task *self, TaskState new_state)
{
    LOCK_RETAINER(&task_lock);

    if (self->state == new_state)
    {
        return;
    }

    self->state = new_state;
}

void task_wait(MAYBE_UNUSED Task *self, MAYBE_UNUSED uint64_t ms)
{
    // TODO: task wait
}

void tasking_initialize(void)
{
    log("Initializing tasking...");

    vec_init(&tasks, alloc_global());

    for (size_t i = 0; i < cpu_count(); i++)
    {
        log("Initializing tasking for cpu: {}", i);
        cpu(i)->schedule.idle = task_create_idle();
        cpu(i)->schedule.current = task_create_boot();
        cpu(i)->schedule.next = cpu(i)->schedule.idle;
    }
}

static size_t running_process_count(void)
{
    size_t running_process_count = 0;

    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state == TASK_STATE_RUNNING)
        {
            running_process_count++;
        }
    }

    return running_process_count;
}

static Task *task_get_most_active(void)
{
    size_t most_active_time = 0;
    Task *current_active = nullptr;

    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state != TASK_STATE_RUNNING || !tasks.data[i]->schedule.is_currently_executed)
        {
            continue;
        }

        auto time_running = (current_tick - tasks.data[i]->schedule.tick_start);

        if (time_running > most_active_time)
        {
            most_active_time = time_running;
            current_active = tasks.data[i];
        }
    }

    return current_active;
}

static bool task_has_waiting_task(void)
{
    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state == TASK_STATE_RUNNING && !tasks.data[i]->schedule.is_currently_executed)
        {
            return true;
        }
    }

    return false;
}

static Task *task_get_most_waiting(void)
{
    size_t most_waiting_time = 0;
    Task *current_waiting = nullptr;

    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state != TASK_STATE_RUNNING || tasks.data[i]->schedule.is_currently_executed)
        {
            continue;
        }

        auto time_waiting = (current_tick - tasks.data[i]->schedule.tick_end);

        if (time_waiting > most_waiting_time)
        {
            most_waiting_time = time_waiting;
            current_waiting = tasks.data[i];
        }
    }

    return current_waiting;
}

static CpuId scheduler_end_task_execution(Task *target)
{
    target->schedule.tick_end = current_tick;
    target->schedule.is_currently_executed = false;
    CpuId cur_cpu = target->schedule.cpu;
    target->schedule.cpu = 0;

    return cur_cpu;
}

static void scheduler_start_task_execution(Task *target, CpuId target_cpu)
{
    target->schedule.tick_start = current_tick;
    target->schedule.is_currently_executed = true;
    target->schedule.cpu = target_cpu;
    cpu(target_cpu)->schedule.next = target;
}

// is the cpu current process idle
static bool scheduler_is_cpu_idle(CpuId id)
{
    return cpu(id)->schedule.next == cpu(id)->schedule.idle;
}

static bool scheduler_any_idle_cpu(void)
{
    for (size_t i = 0; i < cpu_count(); i++)
    {
        if (scheduler_is_cpu_idle(i))
        {
            return true;
        }
    }

    return false;
}

static void scheduler_dispatch_to_idle_cpu(Task *target)
{
    for (size_t i = 0; i < cpu_count(); i++)
    {
        if (scheduler_is_cpu_idle(i))
        {
            scheduler_start_task_execution(target, i);
            break;
        }
    }
}

static void scheduler_dispatch_to_active_cpu(Task *target)
{
    Task *most_active = task_get_most_active();

    CpuId target_cpu = scheduler_end_task_execution(most_active);
    scheduler_start_task_execution(target, target_cpu);
}

static void scheduler_dispatch_all_waiting_task_to_a_cpu(void)
{
    while (task_has_waiting_task())
    {
        scheduler_dispatch_to_idle_cpu(task_get_most_waiting());
    }
}

static void scheduler_update(void)
{
    size_t running = running_process_count();

    // when we have a number of running process under the number of cpu
    if (running <= cpu_count())
    {
        // the rare case where we have 4 cpu and 2 task and 1 waiting, so we put all waiting task to an idle cpu
        // waiting task: {C}
        // running task: [A] [B] [ ] [ ]
        // after:
        // waiting task: none
        // running task: [A] [B] [C] [ ]
        scheduler_dispatch_all_waiting_task_to_a_cpu();

        // note: don't need to update cpu scheduler.next value as they keep the same value as "current" on a context switch (see: scheduler_switch())
        return;
    }

    size_t schedule_count = MIN(cpu_count(), running - cpu_count());

    for (size_t i = 0; i < schedule_count; i++)
    {
        Task *most_waiting = task_get_most_waiting();

        if (scheduler_any_idle_cpu())
        {
            scheduler_dispatch_to_idle_cpu(most_waiting);
        }
        else
        {
            scheduler_dispatch_to_active_cpu(most_waiting);
        }
    }
}

void scheduler_switch_other(void)
{
    for (size_t i = 1; i < cpu_count(); i++)
    {
        if (cpu(i)->schedule.current != cpu(i)->schedule.next)
        {
            cpu_resched_other(i);
        }
    }
}

void scheduler_switch(void)
{
    auto schedule = &cpu_self()->schedule;
    schedule->current = schedule->next;
}

void scheduler_schedule_and_switch(void)
{
    LOCK_RETAINER_TRY(&task_lock)
    {
        current_tick++;

        scheduler_update();
        scheduler_switch_other();
        scheduler_switch();
    }
}
