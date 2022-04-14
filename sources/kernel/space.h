#pragma once

#include "kernel/memory.h"
#include "kernel/vmm.h"

typedef struct
{
    VmmRange range;
    Memory *object;
    size_t offset;
} Mmap;

typedef struct
{
    OBJECT_HEADER;

    BrSpaceFlags flags;
    Lock lock;
    VmmSpace vmm;
    Vec(Mmap) mmaps;
    RangeAlloc alloc;
} Space;

Space *space_create(BrSpaceFlags flags);

void space_ref(Space *self);

void space_deref(Space *self);

void space_switch(Space *self);

typedef Result(BrResult, VmmRange) SpaceResult;

SpaceResult space_map(Space *self, Memory *memory, size_t offset, size_t size, uintptr_t vaddr, BrMemoryFlags flags);

SpaceResult space_unmap(Space *self, VmmRange range);

void space_dump(Space *self);
