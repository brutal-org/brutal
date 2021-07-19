#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/memory.h"

/* --- Memory Object -------------------------------------------------------- */

void memory_object_destroy(MemoryObject *self)
{
    pmm_unused(self->range);
    alloc_free(alloc_global(), self);
}

MemoryObject *memory_object_create(size_t size)
{
    return memory_object_create_pmm(UNWRAP(pmm_alloc(size)));
}

MemoryObject *memory_object_create_pmm(PmmRange range)
{
    auto self = alloc_make(alloc_global(), MemoryObject);
    self->range = range;
    object_init(base_cast(self), OBJECT_MEMORY, (ObjectDtor *)memory_object_destroy);

    return self;
}

void memory_object_ref(MemoryObject *self)
{
    object_ref(base_cast(self));
}

void memory_object_deref(MemoryObject *self)
{
    object_deref(base_cast(self));
}

/* --- Memory Mappings ------------------------------------------------------ */

static void memory_mapping_create(Space *space, VmmRange range, MemoryObject *object)
{
    auto mapping = alloc_make(alloc_global(), MemoryMapping);
    mapping->range = range;
    mapping->object = object;
    vec_push(&space->mappings, mapping);
    vmm_map(space->vmm, range, object->range, BR_MEM_USER | BR_MEM_WRITABLE);
}

static void memory_mapping_destroy(Space *space, MemoryMapping *mapping)
{
    vmm_unmap(space->vmm, mapping->range);
    memory_object_deref(mapping->object);
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

Space *space_create(void)
{
    auto self = alloc_make(alloc_global(), Space);
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

SpaceResult space_map(Space *self, VmmRange range)
{
    LOCK_RETAINER(&self->lock);

    range_alloc_used(&self->alloc, range_cast(USizeRange, range));
    auto object = memory_object_create(range.size);
    memory_mapping_create(self, range, object);

    return OK(SpaceResult, range);
}

SpaceResult space_map_obj(Space *self, VmmRange range, MemoryObject *mobj)
{
    LOCK_RETAINER(&self->lock);

    range_alloc_used(&self->alloc, range_cast(USizeRange, range));

    memory_object_ref(mobj);
    memory_mapping_create(self, range, mobj);

    return OK(SpaceResult, range);
}

SpaceResult space_map_pmm(Space *self, VmmRange range, PmmRange pmm_range)
{
    LOCK_RETAINER(&self->lock);

    range_alloc_used(&self->alloc, range_cast(USizeRange, range));
    auto object = memory_object_create_pmm(pmm_range);
    memory_mapping_create(self, range, object);

    return OK(SpaceResult, range);
}

SpaceResult space_alloc(Space *self, size_t size)
{
    LOCK_RETAINER(&self->lock);

    auto range = range_cast(VmmRange, range_alloc_alloc(&self->alloc, size));
    auto object = memory_object_create(range.size);
    memory_mapping_create(self, range, object);

    return OK(SpaceResult, range);
}

SpaceResult space_alloc_obj(Space *self, MemoryObject *mobj)
{
    LOCK_RETAINER(&self->lock);

    auto range = range_cast(VmmRange, range_alloc_alloc(&self->alloc, mobj->range.size));

    memory_object_ref(mobj);
    memory_mapping_create(self, range, mobj);

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
