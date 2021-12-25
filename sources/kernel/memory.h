#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include "kernel/domain.h"
#include "kernel/heap.h"
#include "kernel/pmm.h"

typedef enum
{
    MEMORY_PMM,
    MEMORY_HEAP,
} MemoryType;

typedef enum
{
    MEMORY_NONE,
    MEMORY_OWNING,
} MemoryFlags;

typedef struct
{
    OBJECT_HEADER;

    MemoryType type;
    MemoryFlags flags;

    union
    {
        HeapRange heap;
        PmmRange pmm;
    };
} Memory;

Memory *memory_heap(HeapRange heap, MemoryFlags flags);

Memory *memory_pmm(PmmRange range, MemoryFlags flags);

void memory_ref(Memory *self);

void memory_deref(Memory *self);

PmmRange memory_range(Memory *self);

uintptr_t memory_base(Memory *self);

size_t memory_size(Memory *self);
