#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include "arch/pmm.h"
#include "arch/vmm.h"

/* --- Memory Object -------------------------------------------------------- */

typedef struct
{
    RefCount refcount;
    PmmRange range;
} MemoryObject;

MemoryObject *memory_object_create(size_t size);

MemoryObject *memory_object_create_pmm(PmmRange range);

void memory_object_ref(MemoryObject *self);

void memory_object_deref(MemoryObject *self);

/* --- Memory Space --------------------------------------------------------- */

typedef struct
{
    VmmRange range;
    MemoryObject *object;
} MemoryMapping;

typedef struct
{
    RefCount refcount;
    Lock lock;

    VmmSpace vmm;
    Vec(MemoryMapping *) mappings;
    RangeAlloc alloc;
} MemorySpace;

MemorySpace *memory_space_create(void);

void memory_space_ref(MemorySpace *self);

void memory_space_deref(MemorySpace *self);

void memory_space_switch(MemorySpace *self);

typedef Result(BrResult, VmmRange) MemorySpaceResult;

MemorySpaceResult memory_space_map(MemorySpace *self, VmmRange range);

MemorySpaceResult memory_space_map_pmm(MemorySpace *self, VmmRange range, PmmRange pmm_range);

MemorySpaceResult memory_space_alloc(MemorySpace *self, size_t size);

void memory_space_unmap(MemorySpace *self, VmmRange range);
