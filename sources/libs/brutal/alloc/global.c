#include <brutal/alloc/heap.h>
#include <embed/mem.h>

static Alloc *heap(void)
{
    static HeapAlloc heap;
    static bool init = false;

    if (!init)
    {
        heap_alloc_init(&heap, NODE_DEFAULT);
        init = true;
    }

    return (Alloc *)&heap;
}

void *alloc_global_acquire(MAYBE_UNUSED Alloc *alloc, size_t size)
{
    embed_mem_lock();
    void *result = alloc_acquire(heap(), size);
    embed_mem_unlock();

    return result;
}

void *alloc_global_resize(MAYBE_UNUSED Alloc *alloc, void *ptr, size_t new_size)
{
    embed_mem_lock();
    void *result = alloc_resize(heap(), ptr, new_size);
    embed_mem_unlock();

    return result;
}

void alloc_global_release(MAYBE_UNUSED Alloc *alloc, void *ptr)
{
    embed_mem_lock();
    alloc_release(heap(), ptr);
    embed_mem_unlock();
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
