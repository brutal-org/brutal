#pragma once

#include "kernel/handhover.h"

#include "ds/bitmap.h"
#include <library/base.h>

void pmm_initialize(const struct handover_mmap *memory_map);

uintptr_t pmm_alloc(size_t page_count);

uintptr_t  pmm_alloc_zero(size_t page_count);

int pmm_free(uintptr_t addr, size_t page_count);
