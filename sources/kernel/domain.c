#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/domain.h"

void domain_destroy(Domain *self)
{
    for (int i = 0; i < slot_count(&self->objects); i++)
    {
        if (slot_used(&self->objects, i))
        {
            object_deref(slot_at(&self->objects, i));
            slot_release(&self->objects, i);
        }
    }

    slot_deinit(&self->objects);
    alloc_free(alloc_global(), self);
}

Domain *domain_create(void)
{
    Domain *self = alloc_make(alloc_global(), Domain);
    slot_init(&self->objects, alloc_global());

    object_init(base$(self), BR_OBJECT_DOMAIN, (ObjectDtor *)domain_destroy);
    return self;
}

void domain_ref(Domain *self)
{
    object_ref(base$(self));
}

void domain_deref(Domain *self)
{
    object_deref(base$(self));
}

BrHandle domain_add(Domain *self, Object *object)
{
    LOCK_RETAINER(&self->lock);

    BrHandle handle = slot_alloc(&self->objects);
    object_ref(object);
    slot_at(&self->objects, handle) = object;

    return handle;
}

BrResult domain_remove(Domain *self, BrHandle handle)
{
    LOCK_RETAINER(&self->lock);

    if (!slot_used(&self->objects, handle))
    {
        return BR_BAD_HANDLE;
    }

    object_deref(slot_at(&self->objects, handle));
    slot_release(&self->objects, handle);

    return BR_SUCCESS;
}

Object *domain_lookup(Domain *self, BrHandle handle, BrObjectType type)
{
    LOCK_RETAINER(&self->lock);

    if (!slot_used(&self->objects, handle))
    {
        return nullptr;
    }

    Object *object = slot_at(&self->objects, handle);

    if (object->type != type && type != BR_OBJECT_ANY)
    {
        return nullptr;
    }

    object_ref(slot_at(&self->objects, handle));

    return slot_at(&self->objects, handle);
}
