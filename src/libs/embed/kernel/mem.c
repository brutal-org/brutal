#include <brutal-debug>
#include <brutal-sync>
#include <embed/mem.h>

#include "kernel/heap.h"

static Lock _lock;

Error embed_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum embed_mem_flag flags)
{
    HeapResult alloc_result = heap_alloc(size);

    if (!alloc_result.succ)
    {
        return ERR_UNDEFINED;
    }

    *out_result = (void *)UNWRAP(alloc_result).base;

    return ERR_SUCCESS;
}

Error embed_mem_release(void *addr, size_t size)
{
    // We assumed that free succeed
    heap_free((HeapRange){(uintptr_t)addr, size});
    return ERR_SUCCESS;
}

static Lock _lock;
static HeapAlloc _heap;

static Alloc *ensure_heap(void)
{
    static bool init = false;
    if (!init)
    {
        heap_alloc_init(&_heap, NODE_DEFAULT);
        init = true;
    }
    return (Alloc *)&_heap;
}

void embed_heap_lock(void)
{
    lock_acquire(&_lock);
}

void embed_heap_unlock(void)
{
    lock_release(&_lock);
}

void *embed_heap_acquire(size_t size)
{
    return alloc_acquire(ensure_heap(), size);
}

void *embed_heap_resize(void *ptr, size_t size)
{
    return alloc_resize(ensure_heap(), ptr, size);
}

void embed_heap_release(void *ptr)
{
    alloc_release(ensure_heap(), ptr);
}
