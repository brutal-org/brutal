#include <brutal/sync.h>
#include "kernel/heap.h"
#include "kernel/pmm.h"
#include "kernel/x86_64/memory/mmap.h"

static Lock heap_lock;

HeapResult heap_alloc(size_t size)
{
    size = ALIGN_UP(size, 4096);

    LOCK_RETAINER(&heap_lock)

    auto pmm_range = TRY(HeapResult, pmm_alloc(size));
    pmm_range.base = mmap_phys_to_io(pmm_range.base);
    auto heap_range = range_cast(HeapRange, pmm_range);
    return OK(HeapResult, heap_range);
}

HeapResult heap_free(HeapRange range)
{
    LOCK_RETAINER(&heap_lock)

    auto pmm_range = range_cast(PmmRange, range);
    pmm_range.base = mmap_io_to_phys(pmm_range.base);
    TRY(HeapResult, pmm_unused(pmm_range));
    return OK(HeapResult, range);
}

PmmRange heap_to_pmm(HeapRange range)
{
    auto pmm_range = range_cast(PmmRange, range);
    pmm_range.base = mmap_io_to_phys(pmm_range.base);
    return pmm_range;
}
