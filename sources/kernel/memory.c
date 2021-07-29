#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/memory.h"

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

    log("Created HeapMemObj({}) at {#p}-{#p}", self->handle, heap.base, heap.base + heap.size - 1);

    return self;
}

MemObj *mem_obj_pmm(PmmRange pmm, MemObjFlags flags)
{
    auto self = alloc_make(alloc_global(), MemObj);

    self->type = MEM_OBJ_PMM;
    self->flags = flags;
    self->pmm = pmm;

    object_init(base_cast(self), OBJECT_MEMORY, (ObjectDtor *)mem_obj_destroy);

    log("Created PmmMemObj({}) at {#p}-{#p}", self->handle, pmm.base, pmm.base + pmm.size - 1);

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
