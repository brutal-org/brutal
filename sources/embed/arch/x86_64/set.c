#include <embed/mem.h>

void *embed_mem_set(void *d, uint8_t s, size_t c)
{
    asm volatile(
        "rep stosb"
        : "=D"(d), "=c"(c)
        : "0"(d), "a"(s), "1"(c)
        : "memory");

    return d;
}
