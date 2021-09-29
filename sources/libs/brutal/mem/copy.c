#include <brutal/mem/funcs.h>
#include <host/asm.h>

void *mem_cpy(void *s1, void const *s2, size_t n)
{
    asm_mem_copy(s1, s2, n);

    return s1;
}
