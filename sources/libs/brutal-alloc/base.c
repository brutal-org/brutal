#include "base.h"

void *alloc_malloc(Alloc *self, size_t size)
{
    void *ptr = alloc_acquire(self, size);
    return ptr;
}

void alloc_free(Alloc *self, void *ptr)
{
    alloc_release(self, ptr);
}

void *alloc_calloc(Alloc *self, size_t size, size_t count)
{
    void *ptr = alloc_acquire(self, size * count);
    mem_set(ptr, 0, size * count);
    return ptr;
}
