#pragma once

"kernel/core/domain.h"
"kernel/core/heap.h"
"kernel/core/pmm.h"

typedef enum
{
    MEMORY_PMM,
    MEMORY_HEAP,
    MEMORY_DMA,
} MemoryType;

typedef struct
{
    OBJECT_HEADER;

    MemoryType type;

    union
    {
        HeapRange heap;
        PmmRange pmm;
    };
} Memory;

Memory *memory_heap(HeapRange heap);

Memory *memory_pmm(PmmRange range);

Memory *memory_dma(PmmRange range);

void memory_ref(Memory *self);

void memory_deref(Memory *self);

PmmRange memory_range(Memory *self);

uintptr_t memory_base(Memory *self);

size_t memory_size(Memory *self);
