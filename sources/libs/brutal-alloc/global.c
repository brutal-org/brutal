#include <embed/mem.h>

#include "heap.h"

void *alloc_global_acquire(MAYBE_UNUSED Alloc *alloc, size_t size)
{
    embed_heap_lock();
    void *result = embed_heap_acquire(size);
    embed_heap_unlock();

    return result;
}

void *alloc_global_resize(MAYBE_UNUSED Alloc *alloc, void *ptr, size_t new_size)
{
    embed_heap_lock();
    void *result = embed_heap_resize(ptr, new_size);
    embed_heap_unlock();

    return result;
}

void alloc_global_release(MAYBE_UNUSED Alloc *alloc, void *ptr)
{
    embed_heap_lock();
    embed_heap_release(ptr);
    embed_heap_unlock();
}

Alloc *alloc_global(void)
{
    static Alloc memory = {
        .acquire = alloc_global_acquire,
        .resize = alloc_global_resize,
        .release = alloc_global_release,
    };

    return (Alloc *)&memory;
}
