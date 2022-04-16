#pragma once

#include <brutal-base>

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

/* --- Entropy Source ------------------------------------------------------- */

size_t m_entropy(void *buf, size_t len);

uint8_t m_entropy8(void);

uint16_t m_entropy16(void);

uint32_t m_entropy32(void);

uint64_t m_entropy64(void);
