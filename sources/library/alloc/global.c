#include <library/alloc/heap.h>
#include <library/task/lock.h>

static struct lock heap_lock;

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

    lock_acquire(&heap_lock);
    void *result = alloc_acquire(heap(), size);
    lock_release(&heap_lock);

    return result;
}

void alloc_global_release(struct alloc *alloc, void *ptr)
{
    UNUSED(alloc);

    lock_acquire(&heap_lock);
    alloc_release(heap(), ptr);
    lock_release(&heap_lock);
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
