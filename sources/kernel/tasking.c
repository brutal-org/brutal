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

task_return_result_t task_create(str_t name, uintptr_t ip, enum task_create_flags flags)
{
    LOCK_RETAINER(&task_lock);

    log("Creating task {}...", name);

    auto task = TRY(task_return_result_t, arch_task_create(ip, flags, 0, 0, 0, 0));

    task->id = task_id++;
    task->name = make_str_fix(str_fix128_t, name);
    task->ip = ip;

    log("Task:{}({}) created...", make_str(&task->name), task->id);

    vec_push(&tasks, task);

    if (flags & TASK_CREATE_START_DIRECT)
    {
        task->state = TASK_RUNNING;
    }

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
        cpu(i)->schedule.idle = UNWRAP(task_create(make_str("idle"), (uintptr_t)&task_idle, 0));
        cpu(i)->schedule.current = UNWRAP(task_create(make_str("boot"), 0, TASK_CREATE_START_DIRECT));
        cpu(i)->schedule.next = cpu(i)->schedule.idle;
    }
}

static size_t running_process_count(void)
{
    size_t running_process_count = 0;
    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state == TASK_RUNNING)
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
        if (tasks.data[i]->state != TASK_RUNNING || !tasks.data[i]->scheduler_state.is_currently_executed)
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

static struct task *task_get_most_waiting()
{
    size_t most_waiting_time = 0;
    struct task *current_waiting = nullptr;

    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state != TASK_RUNNING || tasks.data[i]->scheduler_state.is_currently_executed)
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

// is the cpu current process nullptr or idle
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

static void scheduler_continue_all_cpu(void)
{
    for (size_t i = 0; i < cpu_count(); i++)
    {
        cpu(i)->schedule.next = cpu(i)->schedule.current;
    }
}

static void scheduler_update(void)
{
    size_t running = running_process_count();

    // when we have a number of process under the number of cpu
    if (running <= cpu_count())
    {
        scheduler_continue_all_cpu();
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
