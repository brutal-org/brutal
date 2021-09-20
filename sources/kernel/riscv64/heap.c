#include <brutal/sync.h>
#include "kernel/heap.h"
#include "kernel/mmap.h"
#include "kernel/pmm.h"

HeapResult heap_alloc(MAYBE_UNUSED size_t size)
{
    return (HeapResult){};
}

HeapResult heap_free(HeapRange range)
{
    return OK(HeapResult, range);
}

PmmRange heap_to_pmm(HeapRange range)
{
    return range;
}
