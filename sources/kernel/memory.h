#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include "kernel/domain.h"
#include "kernel/pmm.h"
#include "kernel/vmm.h"

/* --- Memory Object -------------------------------------------------------- */

typedef struct
{
    Object base;

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
    Object base;

    Lock lock;
    VmmSpace vmm;
    Vec(MemoryMapping *) mappings;
    RangeAlloc alloc;
} Space;

Space *space_create(void);

void space_ref(Space *self);

void space_deref(Space *self);

void space_switch(Space *self);

typedef Result(BrResult, VmmRange) SpaceResult;

SpaceResult space_map(Space *self, VmmRange range);

SpaceResult space_map_obj(Space *self, VmmRange range, MemoryObject *mobj);

SpaceResult space_map_pmm(Space *self, VmmRange range, PmmRange pmm_range);

SpaceResult space_alloc(Space *self, size_t size);

SpaceResult space_alloc_obj(Space *self, MemoryObject *mobj);

void space_unmap(Space *self, VmmRange range);
