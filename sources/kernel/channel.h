#pragma once

#include <brutal/ds.h>
#include <syscalls/types.h>
#include "kernel/domain.h"

typedef struct
{
    BrTask sender;
    size_t size;
    Object *object;
    uint8_t data[sizeof(BrMessage) - sizeof(BrMessageHeader)];
} Message;

void message_init(Message *message, BrTask sender, Object *object, uint8_t *data, size_t size);

void message_deinit(Message *message);

typedef struct
{
    Ring(Message) messages;
} Channel;

Channel *channel_create(void);

void channel_destroy(Channel *channel);

bool channel_send(Channel *self, Message *message);

bool channel_recv(Channel *self, Message *message);

bool channel_any(Channel *self);
