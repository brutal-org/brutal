#include <brutal/alloc.h>
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

    vec_push(&self->objects, new_object);
}

void domain_remove(Domain *self, BrHandle handle)
{
    LOCK_RETAINER(&self->lock);

    vec_foreach(object, &self->objects)
    {
        if (object->handle == handle)
        {
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
            refcount_ref(&object->refcount);
            return object;
        }
    }

    return nullptr;
}

static _Atomic BrHandle handles = 1;
static bool initialized = false;
static VecObject global = {};
static Lock lock = {};

void object_init(Object *self, ObjectType type, ObjectDtor *dtor)
{
    refcount_init(&self->refcount);
    self->handle = handles++;
    self->type = type;
    self->dtor = dtor;

    lock_acquire(&lock);
    if (!initialized)
    {
        vec_init(&global, alloc_global());
    }
    vec_push(&global, self);
    lock_release(&lock);
}

void object_ref(Object *self)
{
    LOCK_RETAINER(&lock);
    refcount_ref(&self->refcount);
}

void object_deref(Object *self)
{
    lock_acquire(&lock);
    if (refcount_deref(&self->refcount) == REFCOUNT_0)
    {
        vec_remove(&global, self);
        lock_release(&lock);

        self->dtor(self);
    }
    else
    {
        lock_release(&lock);
    }
}

Object *global_lookup(BrHandle handle, ObjectType type)
{
    LOCK_RETAINER(&lock);

    if (!initialized)
    {
        return nullptr;
    }

    vec_foreach(object, &global)
    {
        if (object->handle == handle && object->type == type)
        {
            refcount_ref(&object->refcount);
            return object;
        }
    }

    return nullptr;
}
