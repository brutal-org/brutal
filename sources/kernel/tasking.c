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

size_t switchable_process_count = 0; // number of process waiting and running

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

static void scheduler_set_running_if_available(struct task *target)
{
    for (size_t i = 0; i < cpu_count(); i++)
    {
        if (cpu(i)->schedule.current == NULL)
        {
            cpu(i)->schedule.current = target;
            return;
        }
    }
}

task_return_result_t task_spawn(uintptr_t ip, bool start_direct)
{
    log("Task creating...");
    LOCK_RETAINER(&task_lock);

    auto task = TRY(task_return_result_t, arch_task_create(ip, 0, 0, 0, 0));

    task->id = task_id++;
    task->ip = ip;
    task->level = TASK_LEVEL_USER; // not used for the moment

    log("Task({}) created...", task->id);

    if (start_direct)
    {
        vec_push(&tasks, task);
        scheduler_set_running_if_available(task);
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
        cpu(i)->schedule.current = NULL;
        cpu(i)->schedule.idle = task_spawn((uintptr_t)task_idle, false)._ok;
    }
}

void update_scheduler_time(void)
{

    switchable_process_count = 0;

    LOCK_RETAINER(&task_lock);
    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state == TASK_WAITING)
        {
            switchable_process_count++;
            tasks.data[i]->scheduler_state.tick_running -= 1;
        }
        else if (tasks.data[i]->state == TASK_RUNNING)
        {
            switchable_process_count++;
            tasks.data[i]->scheduler_state.tick_running += 1;
        }
        else
        {
            tasks.data[i]->scheduler_state.tick_running = 0;
        }
    }
}

struct task *task_get_most_active()
{
    LOCK_RETAINER(&task_lock);
    int most_running_count = 0;
    struct task *curr = NULL;
    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state == TASK_RUNNING && tasks.data[i]->scheduler_state.tick_running > most_running_count)
        {
            most_running_count = tasks.data[i]->scheduler_state.tick_running;
            curr = tasks.data[i];
        }
    }
    return curr;
}

struct task *task_get_most_waiting()
{
    LOCK_RETAINER(&task_lock);
    int most_waiting_count = 0;
    struct task *curr = NULL;
    for (int i = 0; i < tasks.length; i++)
    {
        if (tasks.data[i]->state == TASK_WAITING && tasks.data[i]->scheduler_state.tick_running < most_waiting_count)
        {
            most_waiting_count = tasks.data[i]->scheduler_state.tick_running;
            curr = tasks.data[i];
        }
    }
    return curr;
}

cpu_id_t scheduler_set_task_to_wait(struct task *target)
{
    LOCK_RETAINER(&task_lock);

    // if this is the idle task we don't want to put it as waiting as it is a task that is not in the task list
    if (cpu(target->scheduler_state.task_cpu)->schedule.idle != target)
    {
        target->state = TASK_WAITING;
        target->scheduler_state.tick_running = 0;
        cpu_id_t cur_cpu = target->scheduler_state.task_cpu;
        target->scheduler_state.task_cpu = 0;
        return cur_cpu;
    }

    return target->scheduler_state.task_cpu;
}

void scheduler_set_task_active(struct task *target, cpu_id_t target_cpu)
{
    LOCK_RETAINER(&task_lock);
    target->state = TASK_RUNNING;
    target->scheduler_state.tick_running = 0;
    target->scheduler_state.task_cpu = target_cpu;
    cpu(target_cpu)->schedule.next = target;
}

void scheduler_continue_all_cpu()
{
    for (size_t i = 0; i < cpu_count(); i++)
    {
        if (cpu(i)->schedule.current == NULL)
        {
            cpu(i)->schedule.next = cpu(i)->schedule.idle;
        }

        else
        {
            cpu(i)->schedule.next = cpu(i)->schedule.current;
        }
    }
}

void scheduler_update(void)
{
    update_scheduler_time();
    if (switchable_process_count <= cpu_count()) // the case were we don't need to switch
    {

        return; // early return
    }

    size_t schedule_count = MIN(cpu_count(), switchable_process_count - cpu_count());
    for (size_t i = 0; i < schedule_count; i++)
    {
        struct task *most_active = task_get_most_active();
        struct task *most_waiting = task_get_most_waiting();

        cpu_id_t target_cpu = scheduler_set_task_to_wait(most_active);
        scheduler_set_task_active(most_waiting, target_cpu);
    }
}

void tasking_schedule(void)
{
    if (cpu_self_id() == CPU_SCHEDULER_MANAGER)
    {
        scheduler_update();

        for (size_t i = 0; i < cpu_count(); i++)
        {
            if (i != CPU_SCHEDULER_MANAGER)
            {
                arch_task_switch_for_cpu(i); // send switch to all cpu
            }
        }
    }
    cpu_self()->schedule.current = cpu_self()->schedule.next;
}

uintptr_t tasking_switch(uintptr_t sp)
{
    return sp;
}

uintptr_t tasking_schedule_and_switch(uintptr_t sp)
{
    log("scheduling... {}", cpu_self_id());

    if (task_self() != NULL)
    {
        task_self()->sp = sp; // save stack pointer
    }

    arch_task_save_context(cpu_self()->schedule.current);

    tasking_schedule();

    if (task_self() != NULL)
    {
        sp = task_self()->sp; // load stack pointer
    }

    arch_task_load_context(cpu_self()->schedule.current);

    return tasking_switch(sp);
}
