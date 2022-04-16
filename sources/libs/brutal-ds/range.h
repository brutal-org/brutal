#pragma once

#include "vec.h"

typedef Vec(USizeRange) RangeAlloc;

void range_alloc_init(RangeAlloc *self, Alloc *alloc);

void range_alloc_deinit(RangeAlloc *self);

void range_alloc_used(RangeAlloc *self, USizeRange range);

USizeRange range_alloc_alloc(RangeAlloc *self, size_t size);

void range_alloc_unused(RangeAlloc *self, USizeRange range);

void range_alloc_dump(RangeAlloc *self);
