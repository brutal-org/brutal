#pragma once

#include <brutal/base.h>
#include "syscalls/error.h"

typedef Range(size_t) HeapRange;

typedef Result(BrError, HeapRange) heap_Result;

heap_Result heap_alloc(size_t size);

heap_Result heap_free(HeapRange range);
