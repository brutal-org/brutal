#pragma once

#include <bal //types.h>
#include <brutal/ds.h>
#include <brutal/sync.h>

typedef void ObjectDtor(void *object);

#define OBJECT_HEADER \
    union             \
    {                 \
        BrId id;      \
        Object base;  \
    }

typedef struct
{
    BrId id;
    RefCount refcount;
    BrObjectType type;

    ObjectDtor *dtor;
} Object;

typedef Vec(Object *) VecObject;
typedef Slot(Object *) SlotObject;

void object_init(Object *self, BrObjectType type, ObjectDtor *dtor);

void object_ref(Object *self);

void object_deref(Object *self);

Object *global_lookup(BrId id, BrObjectType type);

static inline void object_cleanup(void *object)
{
    object_deref(*(Object **)object);
    *(Object **)object = nullptr;
}
