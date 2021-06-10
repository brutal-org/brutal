#include <brutal/task/lock.h>
#include "arch/heap.h"
#include "arch/pmm.h"
#include "arch/x86_64/memory/mmap.h"

static struct lock heap_lock;

heap_result_t heap_alloc(size_t size)
{
    LOCK_RETAINER(&heap_lock)

    auto pmm_range = TRY(heap_result_t, pmm_alloc(size));
    pmm_range.base = mmap_phys_to_io(pmm_range.base);
    auto heap_range = range_cast(heap_range_t, pmm_range);
    return OK(heap_result_t, heap_range);
}

heap_result_t heap_free(heap_range_t range)
{
    LOCK_RETAINER(&heap_lock)

    auto pmm_range = range_cast(pmm_range_t, range);
    pmm_range.base = mmap_io_to_phys(pmm_range.base);
    TRY(heap_result_t, pmm_unused(pmm_range));
    return OK(heap_result_t, range);
}
