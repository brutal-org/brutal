#include "kernel/space.h"
#include <brutal-alloc>
#include <brutal-debug>

/* --- Memory Mappings ------------------------------------------------------ */

static void space_mmap_create(Space *space, Memory *object, size_t offset, VmmRange vmm, BrMemoryFlags flags)
{
    memory_ref(object);

    Mmap mapping = {
        .range = vmm,
        .object = object,
        .offset = offset,
    };

    vec_push(&space->mmaps, mapping);

    PmmRange pmm = {memory_base(object) + offset, vmm.size};

    vmm_map(space->vmm, vmm, pmm, BR_MEM_USER | flags);
    vmm_flush(space->vmm, vmm);
}

static void space_mmap_destroy(Space *space, size_t index)
{
    Mmap mapping = vec_at(&space->mmaps, index);

    vmm_unmap(space->vmm, mapping.range);
    memory_deref(mapping.object);
    vec_splice(&space->mmaps, index, 1);
    vmm_flush(space->vmm, mapping.range);
}

/* --- Memory Space --------------------------------------------------------- */

void space_destroy(Space *self)
{
    for (int i = 0; i < self->mmaps.len; i++)
    {
        space_mmap_destroy(self, i);
    }

    range_alloc_deinit(&self->alloc);
    vec_deinit(&self->mmaps);
    vmm_space_destroy(self->vmm);

    alloc_free(alloc_global(), self);
}

Space *space_create(BrSpaceFlags flags)
{
    Space *self = alloc_make(alloc_global(), Space);

    self->flags = flags;
    self->vmm = vmm_space_create();
    vec_init(&self->mmaps, alloc_global());

    range_alloc_init(&self->alloc, alloc_global());
    range_alloc_unused(&self->alloc, (USizeRange){0x400000, 0x800000000000});

    object_init(base$(self), BR_OBJECT_SPACE, (ObjectDtor *)space_destroy);

    return self;
}

void space_ref(Space *self)
{
    object_ref(base$(self));
}

void space_deref(Space *self)
{
    object_deref(base$(self));
}

void space_switch(Space *self)
{
    vmm_space_switch(self->vmm);
}

SpaceResult space_map(Space *self, Memory *memory, size_t offset, size_t size, uintptr_t vaddr, BrMemoryFlags flags)
{
    if (size == 0)
    {
        size = memory_size(memory);
    }

    if (offset + size > memory_size(memory))
    {
        return ERR(SpaceResult, BR_BAD_ARGUMENTS);
    }

    LOCK_RETAINER(&self->lock);

    VmmRange range;

    if (vaddr == 0)
    {
        range = range$(VmmRange, range_alloc_alloc(&self->alloc, size));

        if (range.size == 0)
        {
            return ERR(SpaceResult, BR_OUT_OF_MEMORY);
        }
    }
    else
    {
        range = (VmmRange){vaddr, size};
        range_alloc_used(&self->alloc, range$(USizeRange, range));
    }

    space_mmap_create(self, memory, offset, range, flags);

    return OK(SpaceResult, range);
}

SpaceResult space_unmap(Space *self, VmmRange range)
{
    LOCK_RETAINER(&self->lock);

    for (int i = 0; i < self->mmaps.len; i++)
    {
        Mmap *mapping = &vec_at(&self->mmaps, i);

        if (range_eq(mapping->range, range))
        {
            space_mmap_destroy(self, i);
            return OK(SpaceResult, range);
        }
    }

    return ERR(SpaceResult, BR_BAD_ADDRESS);
}

void space_dump(Space *self)
{
    log$("MemorySpace({})", self->id);

    vec_foreach(mapping, &self->mmaps)
    {
        VmmRange vmm_range = mapping->range;

        log$("\trange:{#p}-{#p} offset:{} size:{}",
             vmm_range.base, vmm_range.base + vmm_range.size - 1, mapping->offset, vmm_range.size);

        if (mapping->object->type == MEMORY_HEAP)
        {
            HeapRange heap_range = mapping->object->heap;
            log$("\t  ->  id:{} range:{#p}-{#p} size:{}", mapping->object->id, heap_range.base, heap_range.base + heap_range.size - 1, heap_range.size);
        }
        else
        {
            PmmRange pmm_range = mapping->object->pmm;
            log$("\t  ->  id:{} range:{#p}-{#p} size:{}", mapping->object->id, pmm_range.base, pmm_range.base + pmm_range.size - 1, pmm_range.size);
        }
    }
}
