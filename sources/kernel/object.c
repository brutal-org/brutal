#include <brutal/alloc.h>
#include "kernel/object.h"

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
            object_ref(object);
            return object;
        }
    }

    return nullptr;
}
