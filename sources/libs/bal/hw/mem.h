#pragma once

#include <bal/abi.h>
#include <brutal/base.h>
#include <brutal/text.h>

typedef struct
{
    size_t len;
    void *buf;
    BrMemObj obj;
} BalMem;

static inline BrMemObj bal_mem_handle(BalMem const *self)
{
    return self->obj;
}

// The shm will take owner ship of the MemObj
MaybeError bal_mem_init_mobj(BalMem *self, BrMemObj obj);

MaybeError bal_mem_init_mobj_offset(BalMem *self, BrMemObj obj, size_t offset, size_t len);

MaybeError bal_mem_init_pmm(BalMem *self, uintptr_t addr, size_t size);

MaybeError bal_mem_init_size(BalMem *self, size_t size);

MaybeError bal_mem_deinit(BalMem *self);
