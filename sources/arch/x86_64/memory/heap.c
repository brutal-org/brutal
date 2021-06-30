#include <brutal/sync.h>
#include "arch/heap.h"
#include "arch/pmm.h"
#include "arch/x86_64/memory/mmap.h"

static Lock heap_lock;

heap_Result heap_alloc(size_t size)
{
    size = ALIGN_UP(size, 4096);

    LOCK_RETAINER(&heap_lock)

    auto pmm_range = TRY(heap_Result, pmm_alloc(size));
    pmm_range.base = mmap_phys_to_io(pmm_range.base);
    auto heap_range = range_cast(HeapRange, pmm_range);
    return OK(heap_Result, heap_range);
}

heap_Result heap_free(HeapRange range)
{
    LOCK_RETAINER(&heap_lock)

    auto pmm_range = range_cast(PmmRange, range);
    pmm_range.base = mmap_io_to_phys(pmm_range.base);
    TRY(heap_Result, pmm_unused(pmm_range));
    return OK(heap_Result, range);
}
