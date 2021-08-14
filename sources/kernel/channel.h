#pragma once

#include <brutal/ds.h>
#include <syscalls/types.h>
#include "kernel/domain.h"

typedef struct
{
    BrTask sender;
    size_t size;
    Object *object;
    uint8_t data[member_size(BrMessage, data)];
} Message;

void message_init(Message *message, BrTask sender, Object *object, uint8_t *data, size_t size);

void message_deinit(Message *message);

typedef struct
{
    Lock lock;
    Ring(Message) messages;
} Channel;

Channel *channel_create(void);

void channel_destroy(Channel *channel);

BrResult channel_send_blocking(Channel *self, Message *message, BrTimeout timeout);

BrResult channel_recv_blocking(Channel *self, Message *message, BrTimeout timeout);

BrResult channel_send(Channel *self, Message *message);

BrResult channel_recv(Channel *self, Message *message);
