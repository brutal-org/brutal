#include <brutal/alloc.h>
#include "kernel/channel.h"
#include "kernel/kernel.h"

void message_init(Message *message, BrTask sender, Object *object, uint8_t *data, size_t size)
{
    object_ref(object);

    message->sender = sender;
    message->object = object;
    message->size = size;

    mem_cpy(message->data, data, size);
}

void message_deinit(Message *message)
{
    object_deref(message->object);
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

bool channel_send(Channel *self, Message *message)
{
    return ring_push(&self->messages, message);
}

bool channel_recv(Channel *self, Message *message)
{
    return ring_pop(&self->messages, message);
}

bool channel_any(Channel *self)
{
    return ring_any(&self->messages);
}
