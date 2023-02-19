#include "kernel/object.h"
#include <brutal-alloc>

static _Atomic BrHandle _id = 1;
static _Atomic bool _init = false;
static VecObject _global = {};
static RwLock _lock = {};

void object_init(Object *self, BrType type, ObjectDtor *dtor)
{
    refcount_init(&self->refcount);

    self->id = _id++;
    self->type = type;
    self->dtor = dtor;

    rwlock_acquire_write(&_lock);

    if (!_init)
    {
        vec_init(&_global, alloc_global());
        _init = true;
    }

    vec_push(&_global, self);

    rwlock_release_write(&_lock);
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

    rwlock_acquire_write(&_lock);

    if (refcount_deref(&self->refcount) == REFCOUNT_0)
    {
        vec_remove(&_global, self);
        rwlock_release_write(&_lock);

        self->dtor(self);
    }
    else
    {
        rwlock_release_write(&_lock);
    }
}

Object *global_lookup(BrId id, BrType type)
{
    if (!_init)
    {
        return nullptr;
    }

    rwlock_acquire_read(&_lock);

    vec_foreach_v(object, &_global)
    {
        if (object->id == id && (object->type == type || type == BR_OBJECT_ANY))
        {
            object_ref(object);
            rwlock_release_read(&_lock);
            return object;
        }
    }

    rwlock_release_read(&_lock);
    return nullptr;
}
