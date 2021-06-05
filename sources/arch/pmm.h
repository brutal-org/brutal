#pragma once

#include "arch/stivale2.h"
#include "ds/bitmap.h"
#include <library/base.h>

void pmm_initialize(struct stivale2_struct_tag_memmap *memory_map);

void *pmm_bitmap_alloc(size_t page_count);

void *pmm_bitmap_alloc_zero(size_t page_count);

int pmm_bitmap_free(void *addr, size_t page_count);