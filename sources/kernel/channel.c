#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/channel.h"
#include "kernel/kernel.h"
#include "kernel/sched.h"

Channel *channel_create(void)
{
    Channel *self = alloc_make(alloc_global(), Channel);
    ring_init(&self->msgs, IPC_CHANNEL_SIZE, alloc_global());
    return self;
}

void channel_destroy(Channel *self)
{
    ring_deinit(&self->msgs);
    alloc_free(alloc_global(), self);
}

static BrResult channel_send_unlock(Channel *self, Envelope const *msg)
{
    if (!ring_push(&self->msgs, msg))
    {
        return BR_CHANNEL_FULL;
    }

    return BR_SUCCESS;
}

static BrResult channel_recv_unlock(Channel *self, Envelope *msg)
{
    if (!ring_pop(&self->msgs, msg))
    {
        return BR_CHANNEL_EMPTY;
    }

    return BR_SUCCESS;
}

static bool channel_wait_send(Channel *self)
{
    if (!lock_try_acquire(&self->lock))
    {
        return false;
    }

    if (ring_full(&self->msgs))
    {
        lock_release(&self->lock);
        return false;
    }

    return true;
}

static bool channel_wait_recv(Channel *self)
{
    if (!lock_try_acquire(&self->lock))
    {
        return false;
    }

    if (!ring_any(&self->msgs))
    {
        lock_release(&self->lock);
        return false;
    }

    return true;
}

BrResult channel_send_blocking(Channel *self, Envelope const *msg, BrDeadline deadline)
{
    BrResult result = BR_SUCCESS;

    if (lock_try_acquire(&self->lock))
    {
        result = channel_send_unlock(self, msg);
        lock_release(&self->lock);
    }

    if (result != BR_CHANNEL_FULL)
    {
        return result;
    }

    result = sched_block((Blocker){
        .function = (BlockerFn *)channel_wait_send,
        .context = self,
        .deadline = deadline,
    });

    if (result != BR_SUCCESS)
    {
        return result;
    }

    result = channel_send_unlock(self, msg);

    lock_release(&self->lock);

    return result;
}

BrResult channel_recv_blocking(Channel *self, Envelope *msg, BrDeadline deadline)
{
    BrResult result = BR_SUCCESS;

    if (lock_try_acquire(&self->lock))
    {
        result = channel_recv_unlock(self, msg);
        lock_release(&self->lock);
    }

    if (result != BR_CHANNEL_EMPTY)
    {
        return result;
    }

    result = sched_block((Blocker){
        .function = (BlockerFn *)channel_wait_recv,
        .context = self,
        .deadline = deadline,
    });

    if (result != BR_SUCCESS)
    {
        return result;
    }

    result = channel_recv_unlock(self, msg);

    lock_release(&self->lock);

    return result;
}

BrResult channel_send_non_blocking(Channel *self, Envelope const *msg)
{
    LOCK_RETAINER(&self->lock);
    return channel_send_unlock(self, msg);
}

BrResult channel_recv_non_blocking(Channel *self, Envelope *msg)
{
    LOCK_RETAINER(&self->lock);
    return channel_recv_unlock(self, msg);
}

BrResult channel_send(Channel *self, Envelope const *msg, BrDeadline deadline, BrIpcFlags flags)
{
    if (flags & BR_IPC_BLOCK)
    {
        return channel_send_blocking(self, msg, deadline);
    }
    else
    {
        return channel_send_non_blocking(self, msg);
    }
}

BrResult channel_recv(Channel *self, Envelope *msg, BrDeadline deadline, BrIpcFlags flags)
{
    if (flags & BR_IPC_BLOCK)
    {
        return channel_recv_blocking(self, msg, deadline);
    }
    else
    {
        return channel_recv_non_blocking(self, msg);
    }
}
