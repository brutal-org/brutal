#include <brutal/task/lock.h>
#include <host/mem.h>
#include "arch/heap.h"

static struct lock mem_lock;

void host_mem_lock(void)
{
    lock_acquire(&mem_lock);
}

void host_mem_unlock(void)
{
    lock_release(&mem_lock);
}

error_t host_mem_acquire(size_t size, void **out_result, enum host_mem_flag flags)
{
    UNUSED(flags);

    auto alloc_result = heap_alloc(size);

    if (!alloc_result.success)
    {
        return ERR_UNDEFINED;
    }

    *out_result = (void *)UNWRAP(alloc_result).base;

    return ERR_SUCCESS;
}

error_t host_mem_commit(void *addr, size_t size)
{
    UNUSED(addr);
    UNUSED(size);
    return ERR_SUCCESS;
}

error_t host_mem_decommit(void *addr, size_t size)
{
    UNUSED(addr);
    UNUSED(size);
    return ERR_SUCCESS;
}

error_t host_mem_release(void *addr, size_t size)
{
    // We assumed that free succeed
    heap_free((heap_range_t){(uintptr_t)addr, size});
    return ERR_SUCCESS;
}
