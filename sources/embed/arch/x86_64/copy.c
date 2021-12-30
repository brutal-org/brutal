#include <embed/mem.h>

void *embed_mem_copy(void *to, void const *from, size_t size)
{
    asm volatile("rep movsb"
                 : "=D"(to),
                   "=S"(from),
                   "=c"(size)
                 : "D"(to),
                   "S"(from),
                   "c"(size)
                 : "memory");

    return to;
}
