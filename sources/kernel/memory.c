#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/memory.h"

/* --- Memory Object -------------------------------------------------------- */

MemoryObject *memory_object_create(size_t size)
{
    return memory_object_create_pmm(UNWRAP(pmm_alloc(size)));
}

MemoryObject *memory_object_create_pmm(PmmRange range)
{
    auto self = alloc_make(alloc_global(), MemoryObject);
    refcount_init(&self->refcount);
    self->range = range;
    return self;
}

void memory_object_ref(MemoryObject *self)
{
    refcount_ref(&self->refcount);
}

void memory_object_deref(MemoryObject *self)
{
    if (refcount_deref(&self->refcount) == REFCOUNT_0)
    {
        pmm_unused(self->range);
        alloc_free(alloc_global(), self);
    }
}

/* --- Memory Mappings ------------------------------------------------------ */

static void memory_mapping_create(MemorySpace *space, VmmRange range, MemoryObject *object)
{
    auto mapping = alloc_make(alloc_global(), MemoryMapping);
    mapping->range = range;
    mapping->object = object;
    vec_push(&space->mappings, mapping);
    vmm_map(space->vmm, range, object->range, BR_MEM_USER | BR_MEM_WRITABLE);
}

static void memory_mapping_destroy(MemorySpace *space, MemoryMapping *mapping)
{
    vmm_unmap(space->vmm, mapping->range);
    memory_object_deref(mapping->object);
    vec_remove(&space->mappings, mapping);
    alloc_free(alloc_global(), mapping);
}

/* --- Memory Space --------------------------------------------------------- */

MemorySpace *memory_space_create(void)
{
    auto self = alloc_make(alloc_global(), MemorySpace);
    refcount_init(&self->refcount);
    self->vmm = vmm_space_create();
    vec_init(&self->mappings, alloc_global());
    range_alloc_init(&self->alloc, alloc_global());

    range_alloc_unused(&self->alloc, (USizeRange){0x400000, 0x7fffffffffff});

    return self;
}

void memory_space_ref(MemorySpace *self)
{
    refcount_ref(&self->refcount);
}

void memory_space_deref(MemorySpace *self)
{
    if (refcount_deref(&self->refcount) == REFCOUNT_0)
    {
        for (int i = 0; i < self->mappings.length; i++)
        {
            memory_mapping_destroy(self, self->mappings.data[i]);
        }

        range_alloc_deinit(&self->alloc);
        vec_deinit(&self->mappings);
        vmm_space_destroy(self->vmm);
    }
}

void memory_space_switch(MemorySpace *self)
{
    vmm_space_switch(self->vmm);
}

MemorySpaceResult memory_space_map(MemorySpace *self, VmmRange range)
{
    LOCK_RETAINER(&self->lock);

    range_alloc_used(&self->alloc, range_cast(USizeRange, range));
    auto object = memory_object_create(range.size);
    memory_mapping_create(self, range, object);

    return OK(MemorySpaceResult, range);
}

MemorySpaceResult memory_space_map_pmm(MemorySpace *self, VmmRange range, PmmRange pmm_range)
{
    LOCK_RETAINER(&self->lock);

    range_alloc_used(&self->alloc, range_cast(USizeRange, range));
    auto object = memory_object_create_pmm(pmm_range);
    memory_mapping_create(self, range, object);

    return OK(MemorySpaceResult, range);
}

MemorySpaceResult memory_space_alloc(MemorySpace *self, size_t size)
{
    LOCK_RETAINER(&self->lock);

    auto range = range_cast(VmmRange, range_alloc_alloc(&self->alloc, size));
    auto object = memory_object_create(range.size);
    memory_mapping_create(self, range, object);

    return OK(MemorySpaceResult, range);
}

void memory_space_unmap(MemorySpace *self, VmmRange range)
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
