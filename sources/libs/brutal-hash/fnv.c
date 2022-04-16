#include "fnv.h"

Fnv32 fnv_32(void const *buf, size_t len, Fnv32 hval)
{
    uint8_t *bp = (uint8_t *)buf;
    uint8_t *be = bp + len;

    while (bp < be)
    {
        hval *= FNV_32_PRIME;
        hval ^= (Fnv32)*bp++;
    }

    return hval;
}

Fnv32 fnv_32a(void const *buf, size_t len, Fnv32 hval)
{
    uint8_t *bp = (uint8_t *)buf;
    uint8_t *be = bp + len;

    while (bp < be)
    {
        hval ^= (Fnv32)*bp++;
        hval *= FNV_32_PRIME;
    }

    return hval;
}

Fnv64 fnv_64(void const *buf, size_t len, Fnv64 hval)
{
    uint8_t *bp = (uint8_t *)buf;
    uint8_t *be = bp + len;

    while (bp < be)
    {

        hval *= FNV_64_PRIME;
        hval ^= (Fnv64)*bp++;
    }

    return hval;
}

Fnv64 fnv_64a(void const *buf, size_t len, Fnv64 hval)
{
    uint8_t *bp = (uint8_t *)buf;
    uint8_t *be = bp + len;

    while (bp < be)
    {
        hval ^= (Fnv64)*bp++;
        hval *= FNV_64_PRIME;
    }

    return hval;
}
