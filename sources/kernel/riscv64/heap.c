#include <brutal/sync.h>
#include "kernel/heap.h"
#include "kernel/mmap.h"
#include "kernel/pmm.h"

static Lock heap_lock;

HeapResult heap_alloc(MAYBE_UNUSED size_t size)
{
    size = ALIGN_UP(size, 4096);

    LOCK_RETAINER(&heap_lock)

    PmmRange pmm_range = TRY(HeapResult, pmm_alloc(size, true));

    return OK(HeapResult, range$(HeapRange, pmm_range));
}

HeapResult heap_free(HeapRange range)
{
    LOCK_RETAINER(&heap_lock)

    TRY(HeapResult, pmm_unused(range$(PmmRange, range)));

    return OK(HeapResult, range);
}

PmmRange heap_to_pmm(HeapRange range)
{
    return range$(PmmRange, range);
}
