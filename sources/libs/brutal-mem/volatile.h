#pragma once

#include <brutal-base>

static inline void volatile_write8(uintptr_t address, uint8_t value)
{
    *((volatile uint8_t *)(address)) = value;
}

static inline uint8_t volatile_read8(uintptr_t address)
{
    return *((volatile uint8_t *)(address));
}

static inline void volatile_write16(uintptr_t address, uint16_t value)
{
    *((volatile uint16_t *)(address)) = value;
}

static inline uint16_t volatile_read16(uintptr_t address)
{
    return *((volatile uint16_t *)(address));
}

static inline void volatile_write32(uintptr_t address, uint32_t value)
{
    *((volatile uint32_t *)(address)) = value;
}

static inline uint32_t volatile_read32(uintptr_t address)
{
    return *((volatile uint32_t *)(address));
}

static inline void volatile_write64(uintptr_t address, uint64_t value)
{
    *((volatile uint64_t *)(address)) = value;
}

static inline uint64_t volatile_read64(uintptr_t address)
{
    return *((volatile uint64_t *)(address));
}
