#pragma once

#include <bal/abi.h>
#include <brutal/base.h>
#include <brutal/text.h>

typedef struct
{
    size_t len;
    void *buf;
    BrHandle handle;
    uintptr_t paddr;
} BalMem;

static inline BrHandle bal_mem_handle(BalMem const *self)
{
    return self->handle;
}

// The shm will take owner ship of the Memory
MaybeError bal_mem_init_mobj(BalMem *self, BrHandle handle);

MaybeError bal_mem_init_mobj_offset(BalMem *self, BrHandle handle, size_t offset, size_t len);

MaybeError bal_mem_init_pmm(BalMem *self, uintptr_t addr, size_t size);

MaybeError bal_mem_init_size(BalMem *self, size_t size);

MaybeError bal_mem_deinit(BalMem *self);

MaybeError bal_memobj_paddr(BrMemObj obj, uintptr_t *paddr);
