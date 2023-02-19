#pragma once

#include <bal/abi.h>
#include <brutal-ds>
#include <brutal-sync>

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
    BrType type;

    ObjectDtor *dtor;
} Object;

typedef Vec(Object *) VecObject;
typedef Slot(Object *) SlotObject;

void object_init(Object *self, BrType type, ObjectDtor *dtor);

void object_ref(Object *self);

void object_deref(Object *self);

Object *global_lookup(BrId id, BrType type);
