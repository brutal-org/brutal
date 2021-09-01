#include <brutal/alloc/heap.h>
#include <host/mem.h>

static Alloc *heap(void)
{
    static bool initialized = false;
    static struct alloc_heap memory;

    if (!initialized)
    {
        alloc_heap_init(&memory);
    }

    return (Alloc *)&memory;
}

void *alloc_global_acquire(MAYBE_UNUSED Alloc *alloc, size_t size)
{
    host_mem_lock();
    void *result = alloc_acquire(heap(), size);
    host_mem_unlock();

    return result;
}

void *alloc_global_resize(MAYBE_UNUSED Alloc *alloc, void *ptr, size_t new_size)
{
    host_mem_lock();
    void *result = alloc_resize(heap(), ptr, new_size);
    host_mem_unlock();

    return result;
}

void alloc_global_release(MAYBE_UNUSED Alloc *alloc, void *ptr)
{
    host_mem_lock();
    alloc_release(heap(), ptr);
    host_mem_unlock();
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
