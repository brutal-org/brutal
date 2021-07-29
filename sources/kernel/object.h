#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include <syscalls/types.h>

typedef enum
{
    OBJECT_NONE,

    OBJECT_MEMORY,
    OBJECT_DOMAIN,
    OBJECT_SPACE,
    OBJECT_TASK,
} ObjectType;

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
    ObjectType type;

    ObjectDtor *dtor;
} Object;

typedef Vec(Object *) VecObject;

void object_init(Object *self, ObjectType type, ObjectDtor *dtor);

void object_ref(Object *self);

void object_deref(Object *self);

Object *global_lookup(BrHandle handle, ObjectType type);
