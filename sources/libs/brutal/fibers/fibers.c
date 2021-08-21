#include <brutal/alloc.h>
#include <brutal/alloc/global.h>
#include <brutal/fibers.h>
#include <brutal/log.h>
#include <brutal/task.h>

static Fiber *current = nullptr;

Fiber *fiber_self(void) { return current; }

static Fiber *fiber_alloc(void)
{
    Fiber *self = alloc_make(alloc_global(), Fiber);
    self->state = FIBER_EMBRYO;

    if (current == nullptr)
    {
        self->next = self;
        self->prev = self;

        current = self;
    }
    else
    {
        self->next = current;
        self->prev = current->prev;

        current->prev->next = self;
        current->prev = self;
    }

    return self;
}

static void fiber_free(Fiber *self)
{
    self->next->prev = self->prev;
    self->prev->next = self->next;

    alloc_free(alloc_global(), self->stack);
    alloc_free(alloc_global(), self);
}

static void fiber_main(void)
{
    current->fn();
    fiber_ret();
}

Fiber *fiber_start(FiberFn fn)
{
    Fiber *self = fiber_alloc();

    static int id = 0;
    self->id = id++;
    self->fn = fn;

    if (fn != nullptr)
    {
        self->stack = alloc_malloc(alloc_global(), FIBER_STACK_SIZE);
        self->ctx.rsp = (uint64_t)self->stack + (FIBER_STACK_SIZE - 8);
        self->ctx.rip = (uint64_t)fiber_main;
        self->ctx.fc_mxcsr = 0;
        self->ctx.fc_x86_cw = 0;
    }

    self->state = FIBER_RUNNING;

    return self;
}

void fiber_block(FiberBlocker blocker)
{
    current->state = FIBER_BLOCKED;
    current->blocker = blocker;

    fiber_yield();
}

void fiber_ret(void)
{
    current->state = FIBER_CANCELING;
    fiber_yield();
}

static bool wait_fiber(Fiber *fiber)
{
    if (fiber->state != FIBER_CANCELING)
    {
        return false;
    }

    fiber->state = FIBER_CANCELED;

    return true;
}

void fiber_await(Fiber *fiber)
{
    fiber_block((FiberBlocker){
        .function = (FiberBlockerFn *)wait_fiber,
        .context = fiber,
        .deadline = -1,
    });
}

static bool fiber_try_unblock(Fiber *self)
{
    if (self->state != FIBER_BLOCKED)
    {
        return true;
    }

    FiberBlocker blocker = self->blocker;

    if (blocker.function &&
        blocker.function(blocker.context))
    {
        self->state = FIBER_RUNNING;
        return true;
    }

    return false;
}

static Fiber *fiber_next(void)
{
    Fiber *next = current->next;

    while (next != current && !fiber_try_unblock(next))
    {
        next = next->next;
    }

    return next;
}

extern void fibers_switch(FibersContext *from, FibersContext *to);

void fiber_yield(void)
{
    Fiber *prev = current;
    Fiber *next = fiber_next();

    if (next == prev)
    {
        return;
    }

    current = next;

    fibers_switch(&prev->ctx, &next->ctx);

    if (prev->state == FIBER_CANCELED)
    {
        fiber_free(prev);
    }
}
