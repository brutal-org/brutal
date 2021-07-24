#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include "kernel/domain.h"
#include "kernel/heap.h"
#include "kernel/pmm.h"
#include "kernel/vmm.h"

/* --- Memory Object -------------------------------------------------------- */

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
    Object base;
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

size_t mem_obj_size(MemObj *self);

/* --- Memory Space --------------------------------------------------------- */

typedef struct
{
    VmmRange range;
    MemObj *object;
    size_t offset;
} MemoryMapping;

typedef struct
{
    Object base;

    BrSpaceFlags flags;
    Lock lock;
    VmmSpace vmm;
    Vec(MemoryMapping *) mappings;
    RangeAlloc alloc;
} Space;

Space *space_create(BrSpaceFlags flags);

void space_ref(Space *self);

void space_deref(Space *self);

void space_switch(Space *self);

typedef Result(BrResult, VmmRange) SpaceResult;

SpaceResult space_map(Space *self, MemObj *mobj, size_t offset, size_t size, uintptr_t vaddr);

void space_unmap(Space *self, VmmRange range);
