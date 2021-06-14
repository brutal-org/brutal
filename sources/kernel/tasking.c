#include <brutal/ds.h>
#include <brutal/log.h>
#include <brutal/sync.h>
#include "arch/cpu.h"
#include "arch/heap.h"
#include "arch/task.h"
#include "kernel/constants.h"
#include "kernel/tasking.h"

static struct lock task_lock = {};
static task_id_t task_id = 0;
static vec_t(struct task *) tasks = {};
size_t current_tick = 0;

void task_idle(void)
{
    while (true)
    {
        asm volatile("hlt");
    }
}

struct task *task_self(void)
{
    return cpu_self()->schedule.current;
}

task_return_result_t task_create(str_t name, enum task_flags flags)
{
    LOCK_RETAINER(&task_lock);

    log("Creating task {}...", name);

    auto task = TRY(task_return_result_t, arch_task_create());

    task->id = task_id++;
    task->name = make_str_fix(str_fix128_t, name);
    task->flags = flags;

    log("Task:{}({}) created...", make_str(&task->name), task->id);

    vec_push(&tasks, task);

    return OK(task_return_result_t, task);
}

void task_start(
    struct task *self,
    uintptr_t ip,
    uintptr_t arg1,
    uintptr_t arg2,
    uintptr_t arg3,
    uintptr_t arg4,
    uintptr_t arg5)
{
    LOCK_RETAINER(&task_lock);

    arch_task_start(self, ip, arg1, arg2, arg3, arg4, arg5);
    self->state = TASK_STATE_RUNNING;
}

struct task *task_create_idle(void)
{
    auto task = UNWRAP(task_create(make_str("idle"), TASK_NONE));
    arch_task_start(task, (uintptr_t)task_idle, 0, 0, 0, 0, 0);
    task->state = TASK_STATE_IDLE;
    return task;
}

struct task *task_create_boot(void)
{
    auto task = UNWRAP(task_create(make_str("bool"), TASK_NONE));
    task_start(task, 0, 0, 0, 0, 0, 0);
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

void task_wait(MAYBE_UNUSED struct task *self, MAYBE_UNUSED uint64_t ms)
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

static struct task *task_get_most_active(void)
{
    size_t most_active_time = 0;
    struct task *current_active = nullptr;

    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state != TASK_STATE_RUNNING || !tasks.data[i]->scheduler_state.is_currently_executed)
        {
            continue;
        }

        auto time_running = (current_tick - tasks.data[i]->scheduler_state.tick_start);

        if (time_running > most_active_time)
        {
            most_active_time = time_running;
            current_active = tasks.data[i];
        }
    }

    return current_active;
}

static bool task_has_waiting_task()
{
    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state == TASK_STATE_RUNNING && !tasks.data[i]->scheduler_state.is_currently_executed)
        {
            return true;
        }
    }

    return false;
}

static struct task *task_get_most_waiting()
{
    size_t most_waiting_time = 0;
    struct task *current_waiting = nullptr;

    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state != TASK_STATE_RUNNING || tasks.data[i]->scheduler_state.is_currently_executed)
        {
            continue;
        }

        auto time_waiting = (current_tick - tasks.data[i]->scheduler_state.tick_end);

        if (time_waiting > most_waiting_time)
        {
            most_waiting_time = time_waiting;
            current_waiting = tasks.data[i];
        }
    }

    return current_waiting;
}

static cpu_id_t scheduler_end_task_execution(struct task *target)
{
    target->scheduler_state.tick_end = current_tick;
    target->scheduler_state.is_currently_executed = false;
    cpu_id_t cur_cpu = target->scheduler_state.cpu;
    target->scheduler_state.cpu = 0;

    return cur_cpu;
}

static void scheduler_start_task_execution(struct task *target, cpu_id_t target_cpu)
{
    target->scheduler_state.tick_start = current_tick;
    target->scheduler_state.is_currently_executed = true;
    target->scheduler_state.cpu = target_cpu;
    cpu(target_cpu)->schedule.next = target;
}

// is the cpu current process idle
static bool scheduler_is_cpu_idle(cpu_id_t id)
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

static void scheduler_dispatch_to_idle_cpu(struct task *target)
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

static void scheduler_dispatch_to_active_cpu(struct task *target)
{
    struct task *most_active = task_get_most_active();

    cpu_id_t target_cpu = scheduler_end_task_execution(most_active);
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
        struct task *most_waiting = task_get_most_waiting();

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

void scheduler_dump(void)
{
    for (size_t i = 0; i < cpu_count(); i++)
    {
        log_unlock("CPU {} will be running {}({})", i, make_str(&cpu(i)->schedule.next->name), cpu(i)->schedule.next->id);
    }
}

void scheduler_switch_other(void)
{
    for (size_t i = 1; i < cpu_count(); i++)
    {
        if (cpu(i)->schedule.current != cpu(i)->schedule.next)
        {
            arch_task_switch_for_cpu(i);
        }
    }
}

uintptr_t scheduler_switch(uintptr_t sp)
{
    auto schedule = &cpu_self()->schedule;

    schedule->current->sp = sp; // save stack pointer
    arch_task_save_context(schedule->current);

    schedule->current = schedule->next;

    sp = schedule->current->sp; // load stack pointer
    arch_task_load_context(schedule->current);

    return sp;
}

uintptr_t scheduler_schedule_and_switch(uintptr_t sp)
{
    //log_unlock("Shed");

    LOCK_RETAINER_TRY(&task_lock)
    {
        current_tick++;

        scheduler_update();
        //scheduler_dump();
        scheduler_switch_other();
        return scheduler_switch(sp);
    }
    else
    {
        return sp;
    }
}
