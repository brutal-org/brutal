#include <brutal/mem/funcs.h>
#include <host/asm.h>

void *mem_set(void *s, uint8_t c, size_t n)
{
    asm_mem_set(s, c, n);
    return s;
}
