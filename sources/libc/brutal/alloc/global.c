#include <brutal/alloc/heap.h>
#include <host/mem.h>

static struct alloc *heap(void)
{
    static bool initialized = false;
    static struct alloc_heap memory;

    if (!initialized)
    {
        alloc_heap_init(&memory);
    }

    return (struct alloc *)&memory;
}

void *alloc_global_acquire(struct alloc *alloc, size_t size)
{
    UNUSED(alloc);

    host_mem_lock();
    void *result = alloc_acquire(heap(), size);
    host_mem_unlock();

    return result;
}

void alloc_global_release(struct alloc *alloc, void *ptr)
{
    UNUSED(alloc);

    host_mem_lock();
    alloc_release(heap(), ptr);
    host_mem_unlock();
}

struct alloc *alloc_global(void)
{
    static struct alloc memory = {
        .acquire = alloc_global_acquire,
        .commit = alloc_no_op,
        .decommit = alloc_no_op,
        .release = alloc_global_release,
    };

    return (struct alloc *)&memory;
}
