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

task_return_result_t task_create(uintptr_t ip, enum task_create_flags flags)
{
    log("Task creating...");

    LOCK_RETAINER(&task_lock);

    auto task = TRY(task_return_result_t, arch_task_create(ip, flags, 0, 0, 0, 0));

    task->id = task_id++;
    task->ip = ip;

    log("Task({}) created...", task->id);

    if (flags & TASK_CREATE_START_DIRECT)
    {
        vec_push(&tasks, task);
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

void task_wait(struct task *self, uint64_t ms)
{
    UNUSED(self);
    UNUSED(ms);
    // TODO: task wait
}

void tasking_initialize(void)
{
    log("Initializing tasking...");

    vec_init(&tasks, alloc_global());

    for (size_t i = 0; i < cpu_count(); i++)
    {
        log("Initializing tasking for cpu: {}", i);
        cpu(i)->schedule.idle = UNWRAP(task_create((uintptr_t)&task_idle, 0));
        cpu(i)->schedule.current = nullptr;
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
        if ((current_tick - tasks.data[i]->scheduler_state.tick_start) > most_active_time)
        {
            most_active_time = (current_tick - tasks.data[i]->scheduler_state.tick_start);
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
        if ((current_tick - tasks.data[i]->scheduler_state.tick_end) > most_waiting_time)
        {
            most_waiting_time = (current_tick - tasks.data[i]->scheduler_state.tick_end);
            current_waiting = tasks.data[i];
        }
    }
    return current_waiting;
}

static cpu_id_t scheduler_end_task_execution(struct task *target)
{

    target->scheduler_state.tick_end = current_tick;
    target->scheduler_state.is_currently_executed = false;
    cpu_id_t cur_cpu = target->scheduler_state.task_cpu;
    target->scheduler_state.task_cpu = 0;

    return cur_cpu;
}

static void scheduler_start_task_execution(struct task *target, cpu_id_t target_cpu)
{
    target->scheduler_state.tick_start = current_tick;
    target->scheduler_state.is_currently_executed = true;
    target->scheduler_state.task_cpu = target_cpu;
    cpu(target_cpu)->schedule.next = target;
}

// is the cpu current process nullptr or idle
static bool scheduler_is_cpu_lazy(cpu_id_t id)
{
    struct task *idle = cpu(id)->schedule.idle;
    return (cpu(id)->schedule.current == nullptr && cpu(id)->schedule.next == nullptr) || (cpu(id)->schedule.next == idle);
}

static size_t scheduler_get_lazy_cpu_count(void)
{
    size_t count = 0;

    for (size_t i = 0; i < cpu_count(); i++)
    {
        if (scheduler_is_cpu_lazy(i))
        {
            count++;
        }
    }

    return count;
}

static size_t scheduler_get_running_cpu_count(void)
{
    size_t count = 0;

    for (size_t i = 0; i < cpu_count(); i++)
    {
        if (!scheduler_is_cpu_lazy(i))
        {
            count++;
        }
    }

    return count;
}

static bool scheduler_add_process_to_a_lazy_cpu(struct task *target)
{
    for (size_t i = 0; i < cpu_count(); i++)
    {
        if (scheduler_is_cpu_lazy(i))
        {
            scheduler_start_task_execution(target, i);
            return true;
        }
    }

    return false;
}

static void scheduler_continue_all_cpu(size_t running_process_count)
{
    size_t running_cpu = scheduler_get_running_cpu_count();
    if (running_cpu >= running_process_count) // the case where for exemple we have 3 process and 4/3 cpu running, don't need to switch
    {
        for (size_t i = 0; i < cpu_count(); i++)
        {
            if (cpu(i)->schedule.current != nullptr)
            {
                cpu(i)->schedule.next = cpu(i)->schedule.current;
            }
        }
        return;
    }

    // here the case where we have 3 process and 1 waiting with 4 cpu

    for (size_t i = 0; i < cpu_count(); i++)
    {

        if (scheduler_is_cpu_lazy(i))
        {
            struct task *target = task_get_most_waiting();
            scheduler_add_process_to_a_lazy_cpu(target);
            running_cpu++;
            if (running_cpu >= running_process_count)
            {
                return;
            }
        }
        else
        {

            cpu(i)->schedule.next = cpu(i)->schedule.current; // don't switch
        }
    }
}

static void scheduler_update(void)
{
    size_t running = running_process_count();

    if (running <= cpu_count()) // when we have a number of process under the number of cpu
    {
        scheduler_continue_all_cpu(running);
        return; // early return
    }

    size_t schedule_count = MIN(cpu_count(), running - cpu_count());

    for (size_t i = 0; i < schedule_count; i++)
    {
        struct task *most_waiting = task_get_most_waiting();

        if (scheduler_get_lazy_cpu_count() != 0) // if a cpu don't do anything (poor guy :^(   ) we give him a process
        {
            scheduler_add_process_to_a_lazy_cpu(most_waiting);
            continue;
        }

        struct task *most_active = task_get_most_active();

        cpu_id_t target_cpu = scheduler_end_task_execution(most_active);
        scheduler_start_task_execution(most_waiting, target_cpu);
    }

    for (size_t i = 1; i < cpu_count(); i++)
    {
        if (cpu(i)->schedule.current != cpu(i)->schedule.next)
        {
            arch_task_switch_for_cpu(i); // send switch to cpu
        }
    }
}

uintptr_t scheduler_switch(uintptr_t sp)
{
    if (task_self() != nullptr)
    {
        task_self()->sp = sp; // save stack pointer
        arch_task_save_context(cpu_self()->schedule.current);
    }

    cpu_self()->schedule.current = cpu_self()->schedule.next;

    if (task_self() != nullptr)
    {
        sp = task_self()->sp; // load stack pointer
        arch_task_load_context(cpu_self()->schedule.current);
    }

    return sp;
}

uintptr_t scheduler_schedule_and_switch(uintptr_t sp)
{
    LOCK_RETAINER_TRY(&task_lock)
    {
        current_tick++;
        scheduler_update();

        return scheduler_switch(sp);
    }
    else
    {
        return sp;
    }
}
