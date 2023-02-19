"kernel/core/task.h"
#include <brutal-alloc>
#include <brutal-debug>
"kernel/core/cpu.h"
"kernel/core/event.h"
"kernel/core/kernel.h"
"kernel/core/sched.h"

Task *task_self(void)
{
    return cpu_self()->current;
}

void task_destroy(Task *task)
{
    log$("Destroying task {}", task->id);

    event_unbind_all(task);
    context_destroy(task->context);
    space_deref(task->space);
    domain_deref(task->domain);
    channel_destroy(task->channel);
    heap_free(task->stack);

    alloc_free(alloc_global(), task);
}

TaskCreateResult task_create(Space *space, BrRight rights, BrTaskFlags flags)
{
    Task *task = alloc_make(alloc_global(), Task);

    task->flags = flags;

    task->rights = rights;
    task->context = context_create();
    space_ref(space);
    task->space = space;
    task->domain = domain_create();
    task->channel = channel_create();

    task->stack = UNWRAP(heap_alloc(KERNEL_STACK_SIZE));
    task->sp = range_end(task->stack);

    object_init(base$(task), BR_OBJECT_TASK, (ObjectDtor *)task_destroy);

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
    if (task_self()->stopped)
    {
        sched_yield();
    }

    task_self()->in_syscall = true;
}

void task_end_syscall(void)
{
    task_self()->in_syscall = false;

    if (task_self()->stopped)
    {
        sched_yield();
    }
}
