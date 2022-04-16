#include "kernel/heap.h"
#include <brutal-sync>
#include "kernel/mmap.h"
#include "kernel/pmm.h"

static Lock _lock;

HeapResult heap_alloc(MAYBE_UNUSED size_t size)
{
    LOCK_RETAINER(&_lock)

    size = align_up$(size, MEM_PAGE_SIZE);
    PmmRange pmm_range = TRY(HeapResult, pmm_alloc(size, true));
    return OK(HeapResult, range$(HeapRange, pmm_range));
}

HeapResult heap_free(HeapRange range)
{
    LOCK_RETAINER(&_lock)

    TRY(HeapResult, pmm_unused(range$(PmmRange, range)));
    return OK(HeapResult, range);
}

PmmRange heap_to_pmm(HeapRange range)
{
    return range$(PmmRange, range);
}
