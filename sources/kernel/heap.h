#pragma once

#include <bal/abi.h>

#include "kernel/pmm.h"

typedef USizeRange HeapRange;

typedef Result(BrResult, HeapRange) HeapResult;

HeapResult heap_alloc(size_t size);

HeapResult heap_alloc_lower(size_t size);

HeapResult heap_free(HeapRange range);

PmmRange heap_to_pmm(HeapRange range);
