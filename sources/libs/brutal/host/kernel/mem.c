#include <brutal/host/mem.h>
#include <brutal/log.h>
#include <brutal/sync.h>
#include "kernel/heap.h"

static Lock mem_lock;

void host_mem_lock(void)
{
    lock_acquire(&mem_lock);
}

void host_mem_unlock(void)
{
    lock_release(&mem_lock);
}

Error host_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum host_mem_flag flags)
{
    auto alloc_result = heap_alloc(size);

    if (!alloc_result.succ)
    {
        return ERR_UNDEFINED;
    }

    *out_result = (void *)UNWRAP(alloc_result).base;

    return ERR_SUCCESS;
}

Error host_mem_release(void *addr, size_t size)
{
    // We assumed that free succeed
    heap_free((HeapRange){(uintptr_t)addr, size});
    return ERR_SUCCESS;
}
