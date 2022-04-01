#pragma once

#include <bruwutal/base/std.h>

typedef struct
{
    uint64_t state;
    uint64_t inc;
} MRand;

void m_rand_init(MRand *self, uint32_t seed);

#define m_rand_init$(SEED)                                               \
    (MRand)                                                              \
    {                                                                    \
        .state = SEED | (uint64_t)SEED << 32,                            \
        .inc = (SEED | ((uint64_t)SEED << 32)) ^ ((uint64_t)SEED << 16), \
    }

uint64_t m_rand_next_u64(MRand *self);

uint32_t m_rand_next_u32(MRand *self);

uint16_t m_rand_next_u16(MRand *self);

uint8_t m_rand_next_u8(MRand *self);
