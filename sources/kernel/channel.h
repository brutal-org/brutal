#pragma once

#include <brutal/ds.h>
#include <syscalls/types.h>
#include "kernel/domain.h"

typedef struct
{
    Lock lock;
    Ring(BrMsg) msgs;
    Ring(Object *) objs;
} Channel;

Channel *channel_create(void);

void channel_destroy(Channel *channel);

BrResult channel_send(Channel *self, BrMsg const *msg, Object *obj, BrTimeout timeout, BrIpcFlags flags);

BrResult channel_recv(Channel *self, BrMsg *msg, Object **obj, BrTimeout timeout, BrIpcFlags flags);
