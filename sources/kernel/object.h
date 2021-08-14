#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include <syscalls/types.h>

typedef void ObjectDtor(void *object);

#define OBJECT_HEADER    \
    union                \
    {                    \
        BrHandle handle; \
        Object base;     \
    }

typedef struct
{
    BrHandle handle;
    RefCount refcount;
    BrObjectType type;

    ObjectDtor *dtor;
} Object;

typedef Vec(Object *) VecObject;

void object_init(Object *self, BrObjectType type, ObjectDtor *dtor);

void object_ref(Object *self);

void object_deref(Object *self);

Object *global_lookup(BrHandle handle, BrObjectType type);

static inline void object_cleanup(void *object)
{
    object_deref(*(Object **)object);
    *(Object **)object = nullptr;
}
