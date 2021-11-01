#pragma once

#include <bal/abi.h>
#include "kernel/domain.h"

typedef struct
{
    BrMsg msg;
    Object *objs[BR_MSG_ARG_COUNT];
} Envelope;

BrResult envelope_send(Envelope *self, BrMsg const *msg, Domain *domain);

BrResult envelope_recv(Envelope *self, BrMsg *msg, Domain *domain);

void envelope_deinit(Envelope *self);
