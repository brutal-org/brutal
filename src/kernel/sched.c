#include "kernel/sched.h"
#include <brutal-alloc>
#include <brutal-debug>
#include "kernel/arch.h"
#include "kernel/cpu.h"

static Lock _lock = {};
static Vec(Task *) _tasks = {};
static Tick _tick = 0;

void sched_initialize(void)
{
    log$("Initializing scheduling...");
    vec_init(&_tasks, alloc_global());
}

Tick sched_tick(void)
{
    return _tick;
}

static void sched_enqueue(Task *task)
{
    task_ref(task);
    vec_push(&_tasks, task);
}

static void sched_dequeue(Task *task)
{
    vec_remove(&_tasks, task);
    task_deref(task);
}

void sched_start(Task *task, uintptr_t ip, uintptr_t sp, BrTaskArgs args)
{
    LOCK_RETAINER(&_lock);

    assert_truth(!task->started);

    context_start(task->context, ip, sp, range_end(task->stack), args, task->flags);
    task->started = true;
    sched_enqueue(task);
}

void sched_stop(Task *task, uintptr_t result)
{
    lock_acquire(&_lock);

    if (task->stopped)
    {
        lock_release(&_lock);
        return;
    }

    task->stopped = true;
    task->result = result;

    lock_release(&_lock);

    if (task == task_self() && !task->in_syscall)
    {
        sched_yield();
    }
}

BrResult sched_block(Blocker blocker)
{
    if (blocker.function && blocker.function(blocker.context))
    {
        return BR_SUCCESS;
    }

    lock_acquire(&_lock);

    task_self()->blocker = blocker;
    task_self()->blocked = true;

    lock_release(&_lock);

    sched_yield();

    return task_self()->blocker.result;
}

Tick sched_deadline(BrTimeout timeout)
{
    if (timeout == BR_TIMEOUT_INFINITY)
    {
        return -1;
    }

    return _tick + timeout;
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

    task->time_end = _tick;

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

    task->time_start = _tick;
    cpu->next = task;
}

bool sched_runnable(Task *task)
{
    return (task->started && !task->blocked) ||
           (task->started && task->stopped && !task->in_syscall);
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
    bool just_stopped = other->time_start == _tick;
    bool runnable_and_not_running = task_runnable(other) && !sched_running(other);
    bool waited_more = best == nullptr || other->time_end < best->time_end;

    return (!just_stopped && runnable_and_not_running && waited_more)
               ? other
               : best;
}

Task *sched_peek(void)
{
    Task *best = nullptr;

    vec_foreach_v(task, &_tasks)
    {
        best = sched_pick(best, task);
    }

    return best;
}

/* --- Dispatch ------------------------------------------------------------- */

static bool sched_dispatch_to_current(Task *task)
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

static bool sched_dispatch_to_idle(Task *task)
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
    return _tick - task->time_start;
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
    vec_foreach_v(task, &_tasks)
    {
        if (!task->blocked)
        {
            continue;
        }

        Blocker *blocker = &task->blocker;

        bool has_reached_function =
            blocker->function &&
            blocker->function(blocker->context);

        bool has_reached_deadline =
            blocker->deadline != (Tick)-1 &&
            blocker->deadline < _tick;

        if (has_reached_function)
        {
            blocker->result = BR_SUCCESS;
            task->blocked = false;
        }
        else if (has_reached_deadline)
        {
            blocker->result = BR_TIMEOUT;
            task->blocked = false;
        }
        else if (task->stopped)
        {
            blocker->result = BR_INTERRUPTED;
            task->blocked = false;
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

void sched_yield(void)
{
    lock_acquire(&_lock);

    sched_updated_blocked();
    sched_next(sched_peek() ?: cpu_self()->idle, cpu_self());

    lock_release(&_lock);

    arch_yield();
}

void sched_schedule(void)
{
    _tick++;

    lock_acquire(&_lock);

    sched_updated_blocked();

    while (sched_dispatch(sched_peek()))
        ;

    lock_release(&_lock);

    sched_switch_other();
}

void sched_switch(void)
{
    LOCK_RETAINER(&_lock);

    cpu_self()->current = cpu_self()->next;
}

/* --- Finalizer ------------------------------------------------------------ */

void sched_finalize(void)
{
    LOCK_RETAINER(&_lock);

    for (int i = 0; i < _tasks.len; i++)
    {
        Task *task = _tasks.data[i];

        if (task->stopped && !sched_runnable(task) && !sched_running(task))
        {
            log$("Finalizing task {}", task->id);
            sched_dequeue(task);
            i--;
        }
    }
}
