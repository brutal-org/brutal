#pragma once

#include <bal/syscalls.h>
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

MaybeError DEPRECATED(balshm_init_str)(BalShm *self, Str str);

Str DEPRECATED(balshm_as_str)(BalShm const *self);

static inline void balshm_cleanup_impl(BalShm *self)
{
    balshm_deinit(self);
}

#define BALSHM_CLEANUP CLEANUP(balshm_cleanup_impl)
