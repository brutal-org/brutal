#pragma once

#include <library/base.h>
#include <library/ds/bitmap.h>

#include "kernel/handhover.h"

void pmm_initialize(struct handover_mmap const *memory_map);

uintptr_t pmm_alloc(size_t page_count);

uintptr_t pmm_alloc_zero(size_t page_count);

int pmm_free(uintptr_t addr, size_t page_count);
