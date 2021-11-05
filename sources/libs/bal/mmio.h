#pragma once

#include <bal/abi/types.h>
#include <bal/io.h>
#include <brutal/mem.h>

typedef struct
{
    uintptr_t base;
    size_t size;
    BrMemObj handle;
} BrMmio;

void br_mmio_init(BrMmio *self, uintptr_t paddr, uintptr_t size);

void br_mmio_alloc(BrMmio *self, uintptr_t size);

void br_mmio_deinit(BrMmio *self);

BalIo br_mmio_range(BrMmio *self);
