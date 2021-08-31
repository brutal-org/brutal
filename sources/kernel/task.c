#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/cpu.h"
#include "kernel/interrupts.h"
#include "kernel/kernel.h"
#include "kernel/sched.h"
#include "kernel/task.h"

Task *task_self(void)
{
    return cpu_self()->current;
}

void task_destroy(Task *task)
{
    log$("Destroying task {}({})", str$(&task->name), task->id);

    irq_unbind_all(task);
    context_destroy(task->context);
    space_deref(task->space);
    domain_deref(task->domain);
    channel_destroy(task->channel);
    heap_free(task->stack);

    alloc_free(alloc_global(), task);
}

TaskCreateResult task_create(Str name, Space *space, BrCap caps, BrTaskFlags flags)
{
    log$("Creating Task {}...", name);

    Task *task = alloc_make(alloc_global(), Task);

    task->name = str_fix$(StrFix128, name);
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

    object_init(base$(task), BR_OBJECT_TASK, (ObjectDtor *)task_destroy);

    log$("Task {}({}) created...", str$(&task->name), task->id);

    return OK(TaskCreateResult, task);
}

void task_ref(Task *self)
{
    object_ref(base$(self));
}

void task_deref(Task *self)
{
    object_deref(base$(self));
}

void task_begin_syscall(void)
{
    if (task_self()->is_stopped)
    {
        sched_yield();
    }

    task_self()->in_syscall = true;
}

void task_end_syscall(void)
{
    task_self()->in_syscall = false;

    if (task_self()->is_stopped)
    {
        sched_yield();
    }
}
