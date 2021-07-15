#pragma once

#include <brutal/base.h>
#include <syscalls/types.h>
#include "arch/pmm.h"

typedef Range(size_t) HeapRange;

typedef Result(BrResult, HeapRange) HeapResult;

HeapResult heap_alloc(size_t size);

HeapResult heap_free(HeapRange range);

PmmRange heap_to_pmm(HeapRange range);
