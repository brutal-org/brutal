#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/memory.h"

/* --- Memory Object -------------------------------------------------------- */

void mem_obj_destroy(MemObj *self)
{
    if (self->flags & MEM_OBJ_OWNING)
    {
        if (self->type == MEM_OBJ_HEAP)
        {
            heap_free(self->heap);
        }
        else
        {
            pmm_unused(self->pmm);
        }
    }

    alloc_free(alloc_global(), self);
}

MemObj *mem_obj_heap(HeapRange heap, MemObjFlags flags)
{
    auto self = alloc_make(alloc_global(), MemObj);
    self->type = MEM_OBJ_HEAP;
    self->flags = flags;
    self->heap = heap;
    object_init(base_cast(self), OBJECT_MEMORY, (ObjectDtor *)mem_obj_destroy);

    return self;
}

MemObj *mem_obj_pmm(PmmRange pmm, MemObjFlags flags)
{
    auto self = alloc_make(alloc_global(), MemObj);
    self->type = MEM_OBJ_PMM;
    self->flags = flags;
    self->pmm = pmm;
    object_init(base_cast(self), OBJECT_MEMORY, (ObjectDtor *)mem_obj_destroy);

    return self;
}

void mem_obj_ref(MemObj *self)
{
    object_ref(base_cast(self));
}

void mem_obj_deref(MemObj *self)
{
    object_deref(base_cast(self));
}

PmmRange mem_obj_range(MemObj *self)
{
    if (self->type == MEM_OBJ_HEAP)
    {
        return heap_to_pmm(self->heap);
    }
    else
    {
        return self->pmm;
    }
}

uintptr_t mem_obj_base(MemObj *self)
{
    return mem_obj_range(self).base;
}

size_t mem_obj_size(MemObj *self)
{
    return mem_obj_range(self).size;
}

/* --- Memory Mappings ------------------------------------------------------ */

static void memory_mapping_create(Space *space, MemObj *object, size_t offset, VmmRange vmm)
{
    mem_obj_ref(object);
    auto mapping = alloc_make(alloc_global(), MemoryMapping);
    mapping->range = vmm;
    mapping->object = object;
    vec_push(&space->mappings, mapping);

    PmmRange pmm = {mem_obj_base(object) + offset, vmm.size};

    vmm_map(space->vmm, vmm, pmm, BR_MEM_USER | BR_MEM_WRITABLE);
}

static void memory_mapping_destroy(Space *space, MemoryMapping *mapping)
{
    vmm_unmap(space->vmm, mapping->range);
    mem_obj_deref(mapping->object);
    vec_remove(&space->mappings, mapping);
    alloc_free(alloc_global(), mapping);
}

/* --- Memory Space --------------------------------------------------------- */

void space_destroy(Space *self)
{
    for (int i = 0; i < self->mappings.length; i++)
    {
        memory_mapping_destroy(self, self->mappings.data[i]);
    }

    range_alloc_deinit(&self->alloc);
    vec_deinit(&self->mappings);
    vmm_space_destroy(self->vmm);

    alloc_free(alloc_global(), self);
}

Space *space_create(BrSpaceFlags flags)
{
    auto self = alloc_make(alloc_global(), Space);

    self->flags = flags;
    self->vmm = vmm_space_create();
    vec_init(&self->mappings, alloc_global());
    range_alloc_init(&self->alloc, alloc_global());

    range_alloc_unused(&self->alloc, (USizeRange){0x400000, 0x7fffffffffff});

    object_init(base_cast(self), OBJECT_SPACE, (ObjectDtor *)space_destroy);

    return self;
}

void space_ref(Space *self)
{
    object_ref(base_cast(self));
}

void space_deref(Space *self)
{
    object_deref(base_cast(self));
}

void space_switch(Space *self)
{
    vmm_space_switch(self->vmm);
}

SpaceResult space_map(Space *self, MemObj *mobj, size_t offset, size_t size, uintptr_t vaddr)
{
    if (size == 0)
    {
        size = mem_obj_size(mobj);
    }

    if (offset + size > mem_obj_size(mobj))
    {
        return ERR(SpaceResult, BR_BAD_ARGUMENTS);
    }

    LOCK_RETAINER(&self->lock);

    VmmRange range;

    if (vaddr == 0)
    {
        auto range = range_cast(VmmRange, range_alloc_alloc(&self->alloc, size));

        if (range.size == 0)
        {
            return ERR(SpaceResult, BR_OUT_OF_MEMORY);
        }
    }
    else
    {
        range = (VmmRange){vaddr, size};
        range_alloc_used(&self->alloc, range_cast(USizeRange, range));
    }

    memory_mapping_create(self, mobj, offset, range);

    return OK(SpaceResult, range);
}

void space_unmap(Space *self, VmmRange range)
{
    LOCK_RETAINER(&self->lock);

    vec_foreach(mapping, &self->mappings)
    {
        if (mapping->range.base == range.base)
        {
            range_alloc_unused(&self->alloc, range_cast(USizeRange, range));
            memory_mapping_destroy(self, mapping);
            return;
        }
    }
}
