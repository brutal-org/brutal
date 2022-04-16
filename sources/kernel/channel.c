#include "kernel/channel.h"
#include <brutal-alloc>
#include <brutal-debug>
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

static BrResult channel_send_unlock(Channel *self, Domain *domain, BrMsg const *msg)
{
    if (ring_full(&self->msgs))
    {
        return BR_CHANNEL_FULL;
    }

    Parcel parcel = {};
    BrResult res = parcel_pack(&parcel, msg, domain);

    if (res != BR_SUCCESS)
    {
        return res;
    }

    ring_push(&self->msgs, &parcel);

    return BR_SUCCESS;
}

static BrResult channel_recv_unlock(Channel *self, Domain *domain, BrMsg *msg)
{
    if (!ring_any(&self->msgs))
    {
        return BR_CHANNEL_EMPTY;
    }

    Parcel parcel = {};
    ring_pop(&self->msgs, &parcel);

    return parcel_unpack(&parcel, msg, domain);
}

typedef struct
{
    Channel *channel;
    Domain *domain;
    BrMsg *msg;
    BrResult result;
} ChannelOp;

static bool channel_wait_send(void *ctx)
{
    ChannelOp *op = ctx;
    Channel *channel = op->channel;

    if (!lock_try_acquire(&channel->lock))
    {
        return false;
    }

    op->result = channel_send_unlock(channel, op->domain, op->msg);
    lock_release(&channel->lock);

    return op->result != BR_CHANNEL_FULL;
}

static bool channel_wait_recv(void *ctx)
{
    ChannelOp *op = ctx;
    Channel *channel = op->channel;

    if (!lock_try_acquire(&channel->lock))
    {
        return false;
    }

    op->result = channel_recv_unlock(channel, op->domain, op->msg);
    lock_release(&channel->lock);

    return op->result != BR_CHANNEL_EMPTY;
}

BrResult channel_send_blocking(Channel *self, Domain *domain, BrMsg const *msg, BrDeadline deadline)
{
    ChannelOp op = {
        .channel = self,
        .domain = domain,
        .msg = (BrMsg *)msg,
    };

    BrResult result = sched_block((Blocker){
        .function = channel_wait_send,
        .context = &op,
        .deadline = deadline,
    });

    if (result != BR_SUCCESS)
    {
        return result;
    }

    return op.result;
}

BrResult channel_recv_blocking(Channel *self, Domain *domain, BrMsg *msg, BrDeadline deadline)
{
    BrResult result = BR_SUCCESS;

    ChannelOp op = {
        .channel = self,
        .domain = domain,
        .msg = (BrMsg *)msg,
    };

    result = sched_block((Blocker){
        .function = channel_wait_recv,
        .context = &op,
        .deadline = deadline,
    });

    if (result != BR_SUCCESS)
    {
        return result;
    }

    return op.result;
}

BrResult channel_send_non_blocking(Channel *self, Domain *domain, BrMsg const *msg)
{
    LOCK_RETAINER(&self->lock);
    return channel_send_unlock(self, domain, msg);
}

BrResult channel_recv_non_blocking(Channel *self, Domain *domain, BrMsg *msg)
{
    LOCK_RETAINER(&self->lock);
    return channel_recv_unlock(self, domain, msg);
}

BrResult channel_send(
    Channel *self,
    Domain *domain,
    BrMsg const *msg,
    BrDeadline deadline,
    BrIpcFlags flags)
{
    if (flags & BR_IPC_BLOCK)
    {
        return channel_send_blocking(self, domain, msg, deadline);
    }
    else
    {
        return channel_send_non_blocking(self, domain, msg);
    }
}

BrResult channel_recv(
    Channel *self,
    Domain *domain,
    BrMsg *msg,
    BrDeadline deadline,
    BrIpcFlags flags)
{
    if (flags & BR_IPC_BLOCK)
    {
        return channel_recv_blocking(self, domain, msg, deadline);
    }
    else
    {
        return channel_recv_non_blocking(self, domain, msg);
    }
}
