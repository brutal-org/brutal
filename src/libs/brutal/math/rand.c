#include <embed/crypto.h>

#include "rand.h"

void m_rand_init(MRand *self, uint32_t seed)
{
    self->state = seed | (uint64_t)seed << 32;
    self->inc = (seed | ((uint64_t)seed << 32)) ^ ((uint64_t)seed << 16);
}

uint64_t m_rand_next_u64(MRand *self)
{
    return m_rand_next_u32(self) | (uint64_t)m_rand_next_u32(self) << 32;
}

uint32_t m_rand_next_u32(MRand *self)
{
    uint64_t oldstate = self->state;

    // Advance internal state
    self->state = oldstate * 6364136223846793005ULL + (self->inc | 1);

    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;

    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint16_t m_rand_next_u16(MRand *self)
{
    return m_rand_next_u32(self);
}

uint8_t m_rand_next_u8(MRand *self)
{
    return m_rand_next_u32(self);
}

/* --- Entropy Source ------------------------------------------------------- */

size_t m_entropy(void *buf, size_t len)
{
    return embed_crypto_entropy(buf, len);
}

uint8_t m_entropy8(void)
{
    uint8_t buf;
    embed_crypto_entropy(&buf, sizeof(buf));
    return buf;
}

uint16_t m_entropy16(void)
{
    uint16_t buf;
    embed_crypto_entropy(&buf, sizeof(buf));
    return buf;
}

uint32_t m_entropy32(void)
{
    uint32_t buf;
    embed_crypto_entropy(&buf, sizeof(buf));
    return buf;
}

uint64_t m_entropy64(void)
{
    uint64_t buf;
    embed_crypto_entropy(&buf, sizeof(buf));
    return buf;
}
