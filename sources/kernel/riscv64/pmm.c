#include <brutal/debug.h>
#include <brutal/ds.h>
#include <brutal/mem.h>
#include <brutal/sync.h>
#include "kernel/arch.h"
#include "kernel/mmap.h"
#include "kernel/pmm.h"

void pmm_initialize(MAYBE_UNUSED Handover const *handover)
{
}

PmmResult pmm_alloc(MAYBE_UNUSED size_t size)
{
    return OK(PmmResult, (PmmRange){});
}

PmmResult pmm_used(MAYBE_UNUSED PmmRange range)
{
    return OK(PmmResult, (PmmRange){});
}

PmmResult pmm_unused(MAYBE_UNUSED PmmRange range)
{
    return OK(PmmResult, (PmmRange){});
}
