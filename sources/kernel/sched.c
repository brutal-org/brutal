#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/arch.h"
#include "kernel/cpu.h"
#include "kernel/sched.h"

static Lock lock = {};
static Vec(Task *) tasks = {};
static _Atomic Tick tick = 0;

void sched_initialize(void)
{
    log("Initializing scheduling...");
    vec_init(&tasks, alloc_global());
}

Tick sched_tick(void)
{
    return tick;
}

static void sched_enqueue(Task *task)
{
    task_ref(task);
    vec_push(&tasks, task);
}

static void sched_dequeue(Task *task)
{
    vec_remove(&tasks, task);
    task_deref(task);
}

void sched_start(Task *task, uintptr_t ip, uintptr_t sp, BrTaskArgs args)
{
    lock_acquire(&lock);

    assert_truth(!task->is_started);

    log("Starting task {}({})", str_cast(&task->name), task->handle);

    context_start(task->context, ip, sp, args, task->flags);
    task->is_started = true;
    sched_enqueue(task);

    lock_release(&lock);
}

void sched_stop(Task *task, uintptr_t result)
{
    lock_acquire(&lock);

    if (task->is_stopped)
    {
        lock_release(&lock);
        return;
    }

    task->is_stopped = true;
    task->result = result;

    lock_release(&lock);

    if (task == task_self() && !task->in_syscall)
    {
        sched_yield();
    }
}

void sched_finalize(void)
{
    lock_acquire(&lock);

    for (int i = 0; i < tasks.length; i++)
    {
        auto task = tasks.data[i];

        if (task->is_stopped && task->cpu == CPU_NONE && !task->in_syscall)
        {
            log("Finalizing task({}) {}", task->handle, str_cast(&task->name));
            sched_dequeue(task);
            i--;
        }
    }

    lock_release(&lock);
}

BrResult sched_block(Blocker blocker)
{
    lock_acquire(&lock);

    task_self()->blocker = blocker;
    task_self()->is_blocked = true;

    lock_release(&lock);

    sched_yield();

    return task_self()->blocker.result;
}

Tick sched_deadline(BrTimeout timeout)
{
    if (timeout == BR_TIMEOUT_INFINITY)
    {
        return -1;
    }

    return tick + timeout;
}

/* --- Peek ----------------------------------------------------------------- */

Task *sched_pick(Task *best, Task *other)
{
    if (!task_runnable(other) || task_running(other) || other->time_end == tick)
    {
        return best;
    }
    else if (best == nullptr || other->time_end < best->time_end)
    {
        return other;
    }
    else
    {
        return best;
    }
}

Task *sched_peek(void)
{
    Task *best = nullptr;

    vec_foreach(task, &tasks)
    {
        best = sched_pick(best, task);
    }

    return best;
}

/* --- Dispatch ------------------------------------------------------------- */

void sched_detach(Task *task)
{
    task->time_end = tick;
    task->cpu = CPU_NONE;
}

void sched_attach(Task *task, Cpu *cpu)
{
    task->cpu = cpu->id;
    task->time_start = tick;
}

void sched_next(Task *task, Cpu *cpu)
{
    if (cpu->next)
    {
        sched_detach(cpu->next);
    }

    cpu->next = nullptr;

    if (task)
    {
        sched_attach(task, cpu);
    }

    cpu->next = task;
}

void sched_current(Task *task, Cpu *cpu)
{
    sched_next(task, cpu);
    cpu->current = task;
}

bool sched_dispatch_to_idle(Task *task)
{
    for (int i = 0; i < cpu_count(); i++)
    {
        if (cpu(i)->next == cpu(i)->idle)
        {
            sched_next(task, cpu(i));
            return true;
        }
    }

    return false;
}

static size_t active_time(Task *task)
{
    return tick - task->time_start;
}

static Cpu *sched_active_cpu(void)
{
    Task *result = nullptr;

    vec_foreach(task, &tasks)
    {
        if (!task_running(task))
        {
            continue;
        }

        if (result == nullptr)
        {
            result = task;
        }
        else if (active_time(result) < active_time(task))
        {
            result = task;
        }
    }

    if (!result || active_time(result) == 0)
    {
        return nullptr;
    }
    else
    {
        return cpu(result->cpu);
    }
}

static bool sched_dispatch_to_active(Task *task)
{
    auto cpu = sched_active_cpu();

    if (!cpu)
    {
        return false;
    }

    sched_next(task, cpu);

    return true;
}

static bool sched_dispatch(Task *task)
{
    if (!task)
    {
        return false;
    }

    return sched_dispatch_to_idle(task) ||
           sched_dispatch_to_active(task);
}

/* --- Core ----------------------------------------------------------------- */

static void sched_updated_blocked(void)
{
    for (int i = 0; i < cpu_count(); i++)
    {
        if (!task_runnable(cpu(i)->current))
        {
            sched_next(cpu(i)->idle, cpu(i));
        }
    }

    vec_foreach(task, &tasks)
    {
        if (!task->is_blocked)
        {
            continue;
        }

        Blocker *blocker = &task->blocker;

        bool has_reached_function =
            blocker->function &&
            blocker->function(blocker->context);

        bool has_reached_deadline =
            blocker->deadline != (Tick)-1 &&
            blocker->deadline < tick;

        if (has_reached_function)
        {
            blocker->result = BR_SUCCESS;
            task->is_blocked = false;
        }
        else if (has_reached_deadline)
        {
            blocker->result = BR_TIMEOUT;
            task->is_blocked = false;
        }
        else if (task->is_stopped)
        {
            blocker->result = BR_INTERRUPTED;
            task->is_blocked = false;
        }
    }
}

static void sched_switch_other(void)
{
    for (int i = 0; i < cpu_count(); i++)
    {
        if (cpu_self_id() != i &&
            cpu(i)->next != cpu(i)->current)
        {
            cpu_resched_other(i);
        }
    }
}

void sched_dump(void)
{
    log_unlock("Tasks:");
    vec_foreach(task, &tasks)
    {
        log_unlock("{}({}) : CPU{}", str_cast(&task->name), task->handle, task->cpu);
    }

    log_unlock("CPUs:");
    for (int i = 0; i < cpu_count(); i++)
    {
        log_unlock("CPU{} : c:{}({}) n:{}({})",
                   i,
                   str_cast(&cpu(i)->current->name),
                   cpu(i)->current->handle,
                   str_cast(&cpu(i)->next->name),
                   cpu(i)->next->handle);
    }
}

void sched_ensure_no_duplicated(void)
{
    for (int i = 0; i < cpu_count(); i++)
    {
        for (int j = 0; j < cpu_count(); j++)
        {
            if (i == j)
            {
                continue;
            }

            if (cpu(i)->next == cpu(j)->next)
            {
                sched_dump();
                panic("sched_ensure_no_duplicated failled (CPU{} and CPU{})!", i, j);
            }
        }
    }
}

void sched_ensure_no_cpu_jump(void)
{
    for (int i = 0; i < cpu_count(); i++)
    {
        for (int j = 0; j < cpu_count(); j++)
        {
            if (i == j)
            {
                continue;
            }

            if (cpu(i)->current == cpu(j)->next)
            {
                sched_dump();
                panic("sched_ensure_no_cpu_jump failled {}({}) jumped from CPU{} to CPU{})!", str_cast(&cpu(i)->current->name), cpu(i)->current->handle, i, j);
            }
        }
    }
}

void sched_yield(void)
{
    lock_acquire(&lock);

    sched_next(sched_peek() ?: cpu_self()->idle, cpu_self());

    sched_ensure_no_cpu_jump();
    sched_ensure_no_duplicated();

    lock_release(&lock);

    arch_yield();
}

void sched_schedule(void)
{
    tick++;

    lock_acquire(&lock);

    sched_updated_blocked();

    while (sched_dispatch(sched_peek()))
        ;

    sched_ensure_no_cpu_jump();
    sched_ensure_no_duplicated();

    lock_release(&lock);

    sched_switch_other();
}

void sched_switch(void)
{
    cpu_self()->current = cpu_self()->next;
}
