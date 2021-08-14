#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/channel.h"
#include "kernel/kernel.h"
#include "kernel/sched.h"

void message_init(Message *message, BrTask sender, Object *object, uint8_t *data, size_t size)
{
    if (object)
    {
        object_ref(object);
    }

    message->sender = sender;
    message->object = object;
    message->size = size;

    mem_cpy(message->data, data, size);
}

void message_deinit(Message *message)
{
    if (message->object)
    {
        object_deref(message->object);
    }
}

Channel *channel_create(void)
{
    auto self = alloc_make(alloc_global(), Channel);
    ring_init(&self->messages, IPC_CHANNEL_SIZE, alloc_global());
    return self;
}

void channel_destroy(Channel *self)
{
    ring_deinit(&self->messages);
    alloc_free(alloc_global(), self);
}

static BrResult channel_send_unlock(Channel *self, Message *message)
{
    if (!ring_push(&self->messages, message))
    {
        return BR_CHANNEL_FULL;
    }

    return BR_SUCCESS;
}

static BrResult channel_recv_unlock(Channel *self, Message *message)
{
    if (!ring_pop(&self->messages, message))
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

    if (!ring_full(&self->messages))
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

    if (!ring_any(&self->messages))
    {
        lock_release(&self->lock);
        return false;
    }

    return true;
}

BrResult channel_send_blocking(Channel *self, Message *message, BrTimeout timeout)
{
    BrResult result = BR_SUCCESS;

    if (lock_try_acquire(&self->lock))
    {
        result = channel_send_unlock(self, message);
        lock_release(&self->lock);
    }

    if (result == BR_CHANNEL_FULL)
    {
        return result;
    }

    result = sched_block((Blocker){
        .function = (BlockerFn *)channel_wait_send,
        .context = self,
        .deadline = sched_deadline(timeout),
    });

    if (result != BR_SUCCESS)
    {
        return result;
    }

    result = channel_send_unlock(self, message);

    lock_release(&self->lock);

    return result;
}

BrResult channel_recv_blocking(Channel *self, Message *message, BrTimeout timeout)
{
    BrResult result = BR_SUCCESS;

    if (lock_try_acquire(&self->lock))
    {
        result = channel_recv_unlock(self, message);
        lock_release(&self->lock);
    }

    if (result != BR_CHANNEL_EMPTY)
    {
        return result;
    }

    result = sched_block((Blocker){
        .function = (BlockerFn *)channel_wait_recv,
        .context = self,
        .deadline = sched_deadline(timeout),
    });

    if (result != BR_SUCCESS)
    {
        return result;
    }

    result = channel_recv_unlock(self, message);

    lock_release(&self->lock);

    return result;
}

BrResult channel_send(Channel *self, Message *message)
{
    LOCK_RETAINER(&self->lock);
    return channel_send_unlock(self, message);
}

BrResult channel_recv(Channel *self, Message *message)
{
    LOCK_RETAINER(&self->lock);
    return channel_recv_unlock(self, message);
}
