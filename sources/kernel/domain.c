#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/domain.h"

void domain_destroy(Domain *self)
{
    vec_foreach(object, &self->objects)
    {
        object_deref(object);
    }

    vec_deinit(&self->objects);
    alloc_free(alloc_global(), self);
}

Domain *domain_create(void)
{
    Domain *self = alloc_make(alloc_global(), Domain);
    vec_init(&self->objects, alloc_global());

    object_init(base_cast(self), OBJECT_DOMAIN, (ObjectDtor *)domain_destroy);
    return self;
}

void domain_ref(Domain *self)
{
    object_ref(base_cast(self));
}

void domain_deref(Domain *self)
{
    object_deref(base_cast(self));
}

void domain_add(Domain *self, Object *new_object)
{
    LOCK_RETAINER(&self->lock);

    vec_foreach(object, &self->objects)
    {
        if (object == new_object)
        {
            return;
        }
    }

    object_ref(new_object);
    vec_push(&self->objects, new_object);
}

void domain_remove(Domain *self, BrHandle handle)
{
    LOCK_RETAINER(&self->lock);

    vec_foreach(object, &self->objects)
    {
        if (object->handle == handle)
        {
            object_deref(object);
            vec_remove(&self->objects, object);
            return;
        }
    }
}

Object *domain_lookup(Domain *self, BrHandle handle, ObjectType type)
{
    LOCK_RETAINER(&self->lock);

    vec_foreach(object, &self->objects)
    {
        if (object->handle == handle && object->type == type)
        {
            object_ref(object);
            return object;
        }
    }

    return nullptr;
}
