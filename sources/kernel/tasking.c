#include "kernel/tasking.h"
#include <brutal-debug>
#include "kernel/arch.h"
#include "kernel/sched.h"
#include "kernel/task.h"

static void idle(void)
{
    while (true)
    {
        arch_idle();
    }
}

static void finalizer(void)
{
    while (true)
    {
        sched_finalize();

        sched_block((Blocker){
            .deadline = sched_deadline(2500),
        });
    }
}

Task *tasking_create_idle(void)
{
    Space *space = space_create(0);
    Task *task = UNWRAP(task_create(space, 0, 0));

    context_start(task->context, (uintptr_t)idle, task->sp, task->sp, (BrTaskArgs){}, task->flags);

    space_deref(space);

    return task;
}

Task *tasking_create_boot(void)
{
    Space *space = space_create(0);
    Task *task = UNWRAP(task_create(space, 0, 0));

    sched_start(task, 0, 0, (BrTaskArgs){});

    space_deref(space);
    task_deref(task);

    return task;
}

void tasking_create_finalizer(void)
{
    Space *space = space_create(0);
    Task *task = UNWRAP(task_create(space, 0, 0));

    sched_start(task, (uintptr_t)finalizer, task->sp, (BrTaskArgs){});

    space_deref(space);
    task_deref(task);
}

void tasking_initialize(void)
{
    log$("Initializing tasking...");

    for (int i = 0; i < cpu_count(); i++)
    {
        log$("Initializing tasking for cpu: {}", i);

        cpu(i)->idle = tasking_create_idle();
        sched_current(tasking_create_boot(), cpu(i));
    }

    tasking_create_finalizer();
}
