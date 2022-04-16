#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-task>

#include "fibers.h"

static Fiber *_current = nullptr;
static Pool _pool = {};

Fiber *fiber_self(void) { return _current; }

static Fiber *fiber_ctor(Alloc *alloc)
{
    Fiber *self = alloc_make(alloc, Fiber);
    self->stack = alloc_malloc(alloc, FIBER_STACK_SIZE);
    return self;
}

static void fiber_dtor(Alloc *alloc, Fiber *fiber)
{
    alloc_free(alloc, fiber->stack);
    alloc_free(alloc, fiber);
}

static Fiber *fiber_alloc(void)
{
    Fiber *self = pool_acquire(&_pool);
    self->state = FIBER_EMBRYO;

    if (_current == nullptr)
    {
        self->next = self;
        self->prev = self;

        _current = self;
    }
    else
    {
        self->next = _current;
        self->prev = _current->prev;

        _current->prev->next = self;
        _current->prev = self;
    }

    return self;
}

static void fiber_free(Fiber *self)
{
    self->next->prev = self->prev;
    self->prev->next = self->next;

    pool_release(&_pool, self);
}

static void _fiber_entry(void)
{
    fiber_ret(_current->fn(_current->args));
}

Fiber *fiber_start(FiberFn fn, void *args)
{
    if (fn == nullptr && args == nullptr)
    {
        pool_init(
            &_pool,
            16,
            (PoolCtorFn *)fiber_ctor,
            (PoolDtorFn *)fiber_dtor,
            alloc_global());
    }

    Fiber *self = fiber_alloc();

    static int id = 0;
    self->id = id++;
    self->fn = fn;
    self->args = args;

    if (fn != nullptr)
    {
        self->ctx.rsp = (uint64_t)self->stack + (FIBER_STACK_SIZE - 8);
        self->ctx.rip = (uint64_t)_fiber_entry;
        self->ctx.fc_mxcsr = 0b1111110000000;
        self->ctx.fc_x86_cw = 0b1100111111;
    }

    self->state = FIBER_RUNNING;

    return self;
}

void fiber_start_and_forget(FiberFn fn, void *args)
{
    Fiber *f = fiber_start(fn, args);
    f->fire_and_forget = true;
}

FiberBlockResult fiber_block(FiberBlocker blocker)
{
    _current->state = FIBER_BLOCKED;
    _current->blocker = blocker;

    fiber_yield();

    return _current->blocker.result;
}

void fiber_sleep(Timeout timeout)
{
    fiber_block((FiberBlocker){
        .deadline = timeout + tick_now(),
    });
}

void fiber_ret(void *ret)
{
    _current->ret = ret;

    if (_current->fire_and_forget)
        _current->state = FIBER_CANCELED;
    else
        _current->state = FIBER_CANCELING;

    fiber_yield();
}

typedef struct
{
    Fiber *fiber;
    void *ret;
} FiberAwaitCtx;

static bool wait_fiber(FiberAwaitCtx *ctx)
{
    if (ctx->fiber->state != FIBER_CANCELING)
    {
        return false;
    }

    ctx->fiber->state = FIBER_CANCELED;
    ctx->ret = ctx->fiber->ret;

    return true;
}

void *fiber_await(Fiber *fiber)
{
    FiberAwaitCtx ctx = {
        fiber,
        nullptr,
    };

    fiber_block((FiberBlocker){
        .function = (FiberBlockerFn *)wait_fiber,
        .context = &ctx,
        .deadline = TIME_TIMEOUT_INFINITY,
    });

    return ctx.ret;
}

static bool fiber_try_unblock(Fiber *self)
{
    if (self->state != FIBER_BLOCKED)
    {
        return true;
    }

    FiberBlocker *blocker = &self->blocker;

    if (blocker->function &&
        blocker->function(blocker->context))
    {
        self->state = FIBER_RUNNING;
        blocker->result = FIBER_SUCCESS;
        return true;
    }

    if (blocker->deadline != TIME_TIMEOUT_INFINITY &&
        blocker->deadline < tick_now())
    {
        blocker->result = FIBER_TIMEOUT;
        self->state = FIBER_RUNNING;
        return true;
    }

    return false;
}

static Fiber *fiber_next(FiberState expected)
{
    Fiber *next = _current->next;

    while (next != _current &&
           !fiber_try_unblock(next) &&
           next->state != expected)
    {
        next = next->next;
    }

    if (next->state != expected)
    {
        return nullptr;
    }

    return next;
}

extern void fibers_switch(FibersContext *from, FibersContext *to);

Fiber *fiber_wait_unblocked(void)
{
    Fiber *next = _current->next;

    while (true)
    {
        if (fiber_try_unblock(next) && (next->state == FIBER_RUNNING || next->state == FIBER_IDLE))
        {
            return next;
        }

        next = next->next;
    }
}

void fiber_free_canceled(void)
{
    Fiber *curr = _current->next;

    while (curr != _current)
    {
        Fiber *next = curr->next;

        if (curr->state == FIBER_CANCELED)
        {
            fiber_free(curr);
        }

        curr = next;
    }
}

void fiber_yield(void)
{
    Fiber *prev = _current;
    Fiber *next = fiber_next(FIBER_RUNNING);

    if (next == nullptr)
    {
        next = fiber_next(FIBER_IDLE);
    }

    if (next == nullptr)
    {
        next = fiber_wait_unblocked();
    }

    if (next == prev)
    {
        return;
    }

    _current = next;

    fibers_switch(&prev->ctx, &next->ctx);
    fiber_free_canceled();
}

Tick fiber_deadline(void)
{
    Tick min = -1;
    Fiber *f = _current;

    do
    {
        if (f->state == FIBER_BLOCKED)
        {
            if (min > f->blocker.deadline)
            {
                min = f->blocker.deadline;
            }
        }

        f = f->next;
    } while (f != _current);

    return min;
}
