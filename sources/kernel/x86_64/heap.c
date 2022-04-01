#include <bruwutal/debug.h>
#include <bruwutal/sync.h>
#include "kernel/heap.h"
#include "kernel/mmap.h"
#include "kernel/pmm.h"

static Lock _lock;

HeapResult heap_alloc(size_t size)
{
    size = align_up$(size, MEM_PAGE_SIZE);

    LOCK_RETAINER(&_lock)

    PmmRange pmm_range = TRY(HeapResult, pmm_alloc(size, true));
    pmm_range.base = mmap_phys_to_io(pmm_range.base);
    HeapRange heap_range = range$(HeapRange, pmm_range);

    return OK(HeapResult, heap_range);
}

HeapResult heap_free(HeapRange range)
{
    LOCK_RETAINER(&_lock)

    PmmRange pmm_range = range$(PmmRange, range);
    pmm_range.base = mmap_io_to_phys(pmm_range.base);
    TRY(HeapResult, pmm_unused(pmm_range));

    return OK(HeapResult, range);
}

PmmRange heap_to_pmm(HeapRange range)
{
    PmmRange pmm_range = range$(PmmRange, range);
    pmm_range.base = mmap_io_to_phys(pmm_range.base);
    return pmm_range;
}
