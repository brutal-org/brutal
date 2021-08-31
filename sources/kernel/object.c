#include <brutal/alloc.h>
#include "kernel/object.h"

static bool initialized = false;
static VecObject global = {};
static Lock lock = {};
static _Atomic BrHandle id = 1;

void object_init(Object *self, BrObjectType type, ObjectDtor *dtor)
{
    refcount_init(&self->refcount);

    self->id = id++;
    self->type = type;
    self->dtor = dtor;

    lock_acquire(&lock);

    if (!initialized)
    {
        vec_init(&global, alloc_global());
        initialized = true;
    }

    vec_push(&global, self);
    lock_release(&lock);
}

void object_ref(Object *self)
{
    refcount_ref(&self->refcount);
}

void object_deref(Object *self)
{
    if (self == nullptr)
    {
        return;
    }

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

Object *global_lookup(BrId id, BrObjectType type)
{
    LOCK_RETAINER(&lock);

    if (!initialized)
    {
        return nullptr;
    }

    vec_foreach(object, &global)
    {
        if (object->id == id && (object->type == type || type == BR_OBJECT_ANY))
        {
            object_ref(object);
            return object;
        }
    }

    return nullptr;
}
