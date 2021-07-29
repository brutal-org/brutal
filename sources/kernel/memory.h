#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include "kernel/domain.h"
#include "kernel/heap.h"
#include "kernel/pmm.h"

typedef enum
{
    MEM_OBJ_PMM,
    MEM_OBJ_HEAP,
} MemObjType;

typedef enum
{
    MEM_OBJ_NONE,
    MEM_OBJ_OWNING,
} MemObjFlags;

typedef struct
{
    OBJECT_HEADER;

    MemObjType type;
    MemObjFlags flags;

    union
    {
        HeapRange heap;
        PmmRange pmm;
    };
} MemObj;

MemObj *mem_obj_heap(HeapRange heap, MemObjFlags flags);

MemObj *mem_obj_pmm(PmmRange range, MemObjFlags flags);

void mem_obj_ref(MemObj *self);

void mem_obj_deref(MemObj *self);

PmmRange mem_obj_range(MemObj *self);

uintptr_t mem_obj_base(MemObj *self);

size_t mem_obj_size(MemObj *self);
