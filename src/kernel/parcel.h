#pragma once

#include <bal/abi.h>
#include "kernel/domain.h"

typedef struct
{
    BrMsg msg;
    Object *objs[BR_MSG_ARG_COUNT];
} Parcel;

BrResult parcel_pack(Parcel *self, BrMsg const *msg, Domain *domain);

BrResult parcel_unpack(Parcel *self, BrMsg *msg, Domain *domain);

void parcel_deinit(Parcel *self);
