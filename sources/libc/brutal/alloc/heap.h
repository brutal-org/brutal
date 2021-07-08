#pragma once

#include <brutal/alloc/base.h>

struct alloc_heap
{
    Alloc base;
    struct alloc_major *root; // The root memory block acquired from the system.
    struct alloc_major *best; // The major with the most free memory.
};

void alloc_heap_init(struct alloc_heap *alloc);
