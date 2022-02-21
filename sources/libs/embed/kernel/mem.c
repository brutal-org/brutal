#include <brutal/debug.h>
#include <brutal/sync.h>
#include <embed/mem.h>
#include "kernel/heap.h"

static Lock _lock;

void embed_mem_lock(void)
{
    lock_acquire(&_lock);
}

void embed_mem_unlock(void)
{
    lock_release(&_lock);
}

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
