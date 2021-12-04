#include <brutal/alloc.h>
#include "kernel/object.h"

static _Atomic BrHandle _id = 1;
static bool _init = false;
static VecObject _global = {};
static Lock _lock = {};

void object_init(Object *self, BrObjectType type, ObjectDtor *dtor)
{
    refcount_init(&self->refcount);

    self->id = _id++;
    self->type = type;
    self->dtor = dtor;

    lock_acquire(&_lock);

    if (!_init)
    {
        vec_init(&_global, alloc_global());
        _init = true;
    }

    vec_push(&_global, self);

    lock_release(&_lock);
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

    lock_acquire(&_lock);

    if (refcount_deref(&self->refcount) == REFCOUNT_0)
    {
        vec_remove(&_global, self);
        lock_release(&_lock);

        self->dtor(self);
    }
    else
    {
        lock_release(&_lock);
    }
}

Object *global_lookup(BrId id, BrObjectType type)
{
    LOCK_RETAINER(&_lock);

    if (!_init)
    {
        return nullptr;
    }

    vec_foreach(object, &_global)
    {
        if (object->id == id && (object->type == type || type == BR_OBJECT_ANY))
        {
            object_ref(object);
            return object;
        }
    }

    return nullptr;
}
