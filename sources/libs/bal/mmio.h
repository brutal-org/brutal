#pragma once

#include <bal/abi/types.h>
#include <brutal/mem.h>

typedef struct
{
    uintptr_t base;
    size_t size;
    BrMemObj handle;
} BrMmio;

void br_mmio_init(BrMmio *self, uintptr_t paddr, uintptr_t size);

void br_mmio_deinit(BrMmio *self);

static inline void br_mmio_write8(BrMmio *self, size_t offset, uint8_t value)
{
    volatile_write8(self->base + offset, value);
}

static inline uint8_t br_mmio_read8(BrMmio *self, size_t offset)
{
    return volatile_read8(self->base + offset);
}

static inline void br_mmio_write16(BrMmio *self, size_t offset, uint16_t value)
{
    volatile_write16(self->base + offset, value);
}

static inline uint16_t br_mmio_read16(BrMmio *self, size_t offset)
{
    return volatile_read16(self->base + offset);
}

static inline void br_mmio_write32(BrMmio *self, size_t offset, uint32_t value)
{
    volatile_write32(self->base + offset, value);
}

static inline uint32_t br_mmio_read32(BrMmio *self, size_t offset)
{
    return volatile_read32(self->base + offset);
}

static inline void br_mmio_write64(BrMmio *self, size_t offset, uint64_t value)
{
    volatile_write64(self->base + offset, value);
}

static inline uint64_t br_mmio_read64(BrMmio *self, size_t offset)
{
    return volatile_read64(self->base + offset);
}
