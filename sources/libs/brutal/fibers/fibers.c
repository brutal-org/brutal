#include <brutal/alloc.h>
#include <brutal/alloc/global.h>
#include <brutal/fibers.h>
#include <brutal/log.h>
#include <brutal/task.h>

Fiber *root_fiber;
Fiber *curr_fiber;

int fiber_next_uid = 0;
bool next_switch_must_delete_fiber = false;

extern void fibers_switch_context(fibers_reg *save, fibers_reg *load);

static void fiber_delete(Fiber *fiber)
{
    alloc_free(alloc_global(), fiber->stack);
    alloc_free(alloc_global(), fiber);
}

static void delete_free_fiber()
{
    Fiber *target = root_fiber;

    while (target != nullptr)
    {
        if (target->next != nullptr && target->next->state == FIBER_MUST_BE_DELETED)
        {
            // we can't delete our own fiber we must ask to the next fiber to delete it
            if (target->next == curr_fiber)
            {
                next_switch_must_delete_fiber = true;
                fiber_switch();
            }

            Fiber *to_delete = target->next;
            target->next = target->next->next; // delete the next one
            fiber_delete(to_delete);
        }

        target = target->next;
    }
}

static Fiber *alloc_fiber()
{
    if (root_fiber == nullptr)
    {
        root_fiber = (Fiber *)alloc_malloc(alloc_global(), sizeof(struct fiber));
        root_fiber->next = nullptr;
        return root_fiber;
    }

    Fiber *target = root_fiber;

    while (target->next != nullptr)
    {
        target = target->next;
    }

    target->next = (Fiber *)alloc_malloc(alloc_global(), sizeof(struct fiber));
    target->next->next = nullptr;
    return target->next;
}

// get the next fiber to run
static Fiber *next_fiber(Fiber *from)
{
    Fiber *target = from;

    while (target != nullptr)
    {
        if (target->state == FIBER_RUNNING)
        {
            return target;
        }
        target = target->next;
    }

    if (target == nullptr && from != root_fiber)
    {
        return next_fiber(root_fiber); // if we reached the end of the list from  [ from {...} ] reloop from 0 instead [ {...} from {...}]
    }
    else
    {
        todo("implement fibers return result");
        log("no more fibers exiting \n");
        task_exit(task_self(), 0);
    }

    return from;
}

void fiber_switch()
{
    Fiber *next = next_fiber(curr_fiber->next);

    if (next != curr_fiber) // don't switch if we doesn't need to
    {
        fibers_switch_context(&curr_fiber->regs, &next->regs);

        curr_fiber = next;

        if (next_switch_must_delete_fiber)
        {
            next_switch_must_delete_fiber = false;
            delete_free_fiber();
        }
    }
}

Fiber *fiber_init(fiber_function call)
{
    Fiber *f = alloc_fiber();

    if (call != nullptr)
    {
        f->stack = alloc_malloc(alloc_global(), FIBER_STACK_SIZE);
        f->regs.rsp = (uint64_t)f->stack + (FIBER_STACK_SIZE - 128);
        f->regs.rip = (uint64_t)call;
    }
    else
    {
        curr_fiber = f;
    }

    f->uid = fiber_next_uid++;
    f->state = FIBER_RUNNING;

    return f;
}

void pause_fiber(Fiber *fiber)
{
    fiber->state = FIBER_SUSPENDED;
    if (fiber == curr_fiber)
    {
        fiber_switch();
    }
}

void stop_fiber(Fiber *fiber)
{
    fiber->state = FIBER_MUST_BE_DELETED;

    delete_free_fiber();
}

Fiber *fiber_self()
{
    return curr_fiber;
}
