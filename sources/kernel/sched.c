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
    log$("Initializing scheduling...");
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
    LOCK_RETAINER(&lock);

    assert_truth(!task->is_started);

    log$("Starting task {}({})", str$(&task->name), task->handle);

    context_start(task->context, ip, sp, range_end(task->stack), args, task->flags);
    task->is_started = true;
    sched_enqueue(task);
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

/* --- Attach/Detach -------------------------------------------------------- */

Cpu *sched_cpu(Task *task)
{
    for (int i = 0; i < cpu_count(); i++)
    {
        if (cpu(i)->next == task)
        {
            return cpu(i);
        }
    }

    return nullptr;
}

void sched_detach(Task *task)
{
    if (!task)
    {
        return;
    }

    task->time_end = tick;

    for (int i = 0; i < cpu_count(); i++)
    {
        if (cpu(i)->next == task)
        {
            cpu(i)->next = nullptr;
        }
    }
}

void sched_attach(Task *task, Cpu *cpu)
{
    if (!task)
    {
        return;
    }

    task->time_start = tick;
    cpu->next = task;
}

bool sched_runnable(Task *task)
{
    return (task->is_started && !task->is_blocked) ||
           (task->is_started && task->is_stopped && !task->in_syscall);
}

bool sched_running(Task *task)
{
    return sched_cpu(task) != nullptr;
}

void sched_next(Task *task, Cpu *cpu)
{
    sched_detach(cpu->next);
    sched_attach(task, cpu);
}

void sched_current(Task *task, Cpu *cpu)
{
    sched_next(task, cpu);
    cpu->current = task;
}

/* --- Peek ----------------------------------------------------------------- */

Task *sched_pick(Task *best, Task *other)
{
    if (!task_runnable(other) || sched_running(other) || other->time_end == tick)
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

bool sched_dispatch_to_current(Task *task)
{
    for (int i = 0; i < cpu_count(); i++)
    {
        if (cpu(i)->current == task)
        {
            sched_next(task, cpu(i));
            return true;
        }
    }

    return false;
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

static bool sched_dispatch_to_active(Task *task)
{
    Cpu *best = cpu(0);

    for (int i = 0; i < cpu_count(); i++)
    {
        if (active_time(best->next) < active_time(cpu(i)->next))
        {
            best = cpu(i);
        }
    }

    if (active_time(best->next) == 0)
    {
        return false;
    }

    sched_next(task, best);

    return true;
}

static bool sched_dispatch(Task *task)
{
    if (!task)
    {
        return false;
    }

    return sched_dispatch_to_current(task) ||
           sched_dispatch_to_idle(task) ||
           sched_dispatch_to_active(task);
}

/* --- Core ----------------------------------------------------------------- */

static void sched_updated_blocked(void)
{
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

    for (int i = 0; i < cpu_count(); i++)
    {
        if (!task_runnable(cpu(i)->current))
        {
            sched_next(cpu(i)->idle, cpu(i));
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
        Cpu *cpu = sched_cpu(task);
        log_unlock("{}({}) : CPU{}", str$(&task->name), task->handle, cpu ? cpu->id : -1);
    }

    log_unlock("CPUs:");
    for (int i = 0; i < cpu_count(); i++)
    {
        log_unlock("CPU{} : c:{}({}) n:{}({})",
                   i,
                   str$(&cpu(i)->current->name),
                   cpu(i)->current->handle,
                   str$(&cpu(i)->next->name),
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
                panic$("sched_ensure_no_duplicated failled (CPU{} and CPU{})!", i, j);
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
                panic$("sched_ensure_no_cpu_jump() failled {}:{#p}({}) jumped from CPU{} to CPU{} taking the place of {}:{#p}({})!",
                       str$(&cpu(i)->current->name), (uintptr_t)cpu(i)->next, cpu(i)->current->handle,
                       i, j,
                       str$(&cpu(j)->next->name), (uintptr_t)cpu(j)->next, cpu(j)->next->handle);
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
    LOCK_RETAINER(&lock);

    cpu_self()->current = cpu_self()->next;
}

/* --- Finalizer ------------------------------------------------------------ */

void sched_finalize(void)
{
    LOCK_RETAINER(&lock);

    for (int i = 0; i < tasks.length; i++)
    {
        auto task = tasks.data[i];

        if (task->is_stopped && !sched_runnable(task) && !sched_running(task))
        {
            log$("Finalizing task({}) {}", task->handle, str$(&task->name));
            sched_dequeue(task);
            i--;
        }
    }
}
