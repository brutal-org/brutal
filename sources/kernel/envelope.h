#pragma once

#include <syscalls/types.h>
#include "kernel/domain.h"

typedef struct
{
    BrMsg msg;
    Object *objs[BR_MSG_ARG_COUNT];
} Envelope;

BrResult envelope_send(Envelope *self, BrMsg const *msg, Domain *domain);

BrResult envelope_recv(Envelope *self, BrMsg *msg, Domain *domain);

void envelope_deinit(Envelope *self);

static inline void envelope_cleanup(Envelope *self)
{
    envelope_deinit(self);
}
