#pragma once

#include <brutal/ds.h>
#include "kernel/envelope.h"

typedef struct
{
    Lock lock;
    Ring(Envelope) msgs;
} Channel;

Channel *channel_create(void);

void channel_destroy(Channel *channel);

BrResult channel_send(Channel *self, Domain *domaine, BrMsg const *msg, BrDeadline deadline, BrIpcFlags flags);

BrResult channel_recv(Channel *self, Domain *domaine, BrMsg *msg, BrDeadline deadline, BrIpcFlags flags);
