#pragma once

#include <bal/abi.h>
#include <brutal/base.h>
#include <brutal/text.h>

typedef struct
{
    size_t len;
    void *buf;
    BrMemObj obj;
} BalShm;

static inline BrMemObj balshm_handle(BalShm const *self)
{
    return self->obj;
}

// The shm will take owner ship of the MemObj
MaybeError balshm_init_mobj(BalShm *self, BrMemObj obj);

MaybeError balshm_init_size(BalShm *self, size_t size);

MaybeError balshm_deinit(BalShm *self);
