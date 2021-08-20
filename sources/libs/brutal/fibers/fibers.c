#include <brutal/alloc.h>
#include <brutal/alloc/global.h>
#include <brutal/fibers.h>
#include <brutal/log.h>
#include <brutal/task.h>

Fiber *root_fiber;
Fiber *curr_fiber;

static int fiber_next_uid = 0;
static bool next_switch_must_delete_fiber = false;

// instead of checking each fibers for block every switch, only check them when we know there is at least one blocked
static bool has_blocked_fibers = false;

extern void fibers_switch_context(FibersContext *save, FibersContext *load);

static void fiber_delete(Fiber *fiber)
{
    alloc_free(alloc_global(), fiber->stack);
    alloc_free(alloc_global(), fiber);
}

static void fiber_update_blocked(void)
{
    if (!has_blocked_fibers)
    {
        return;
    }
    has_blocked_fibers = false;
    Fiber *target = root_fiber;

    while (target != nullptr)
    {
        if (target->state == FIBER_SUSPENDED)
        {
            has_blocked_fibers = true;
            if (target->block.function(target->block.context))
            {
                target->state = FIBER_RUNNING;
            }
        }

        target = target->next;
    }
}

static void delete_free_fiber(void)
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
                fiber_yield();
            }

            Fiber *to_delete = target->next;
            target->next = target->next->next; // delete the next one
            fiber_delete(to_delete);
        }

        target = target->next;
    }
}

static Fiber *alloc_fiber(void)
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
        while (true)
        {
        }
    }
}

void fiber_yield(void)
{
    // update before so if a fiber is unlocked we can directly switch into it
    fiber_update_blocked();
    Fiber *next = next_fiber(curr_fiber->next);

    if (next != curr_fiber) // don't switch if we doesn't need to
    {
        // We need to save it here because if we keep curr_fiber and switch it later, it will use the one from the previous fiber stack resulting in a loop
        Fiber *prev_next = curr_fiber;
        curr_fiber = next;

        fibers_switch_context(&prev_next->regs, &next->regs);

        if (next_switch_must_delete_fiber)
        {
            next_switch_must_delete_fiber = false;
            delete_free_fiber();
        }
    }
}

Fiber *fiber_start(fiber_function call)
{
    Fiber *f = alloc_fiber();

    if (call != nullptr)
    {
        f->stack = alloc_malloc(alloc_global(), FIBER_STACK_SIZE);
        f->regs.rsp = (uint64_t)f->stack + (FIBER_STACK_SIZE - 8);
        f->regs.rip = (uint64_t)call;
        f->regs.fc_mxcsr = 0;
        f->regs.fc_x86_cw = 0;
    }
    else
    {
        curr_fiber = f;
    }

    f->uid = fiber_next_uid++;
    f->state = FIBER_RUNNING;

    return f;
}

void fiber_pause(Fiber *fiber, FiberBlocker blocker)
{
    has_blocked_fibers = true;
    fiber->state = FIBER_SUSPENDED;
    fiber->block = blocker;
    if (fiber == curr_fiber)
    {
        fiber_yield();
    }
}

void fiber_stop(Fiber *fiber)
{
    fiber->state = FIBER_MUST_BE_DELETED;

    delete_free_fiber();
}

Fiber *fiber_self(void)
{
    return curr_fiber;
}
