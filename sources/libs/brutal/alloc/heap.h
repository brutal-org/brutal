#pragma once

#include <brutal/alloc/base.h>

typedef struct
{
    Alloc base;
    struct alloc_major *root; // The root memory block acquired from the system.
    struct alloc_major *best; // The major with the most free memory.
} HeapAlloc;

void heap_alloc_init(HeapAlloc *alloc);

void heap_alloc_deinit(HeapAlloc *alloc);
