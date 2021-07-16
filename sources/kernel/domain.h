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

typedef struct
{
    RefCount refcount;
    BrHandle handle;
    ObjectType type;

    ObjectDtor *dtor;
} Object;

typedef Vec(Object *) VecObject;

typedef struct
{
    Object base;

    Lock lock;
    VecObject objects;
} Domain;

typedef Result(BrResult, Object *) ObjectLookupResult;

Domain *domain_create(void);

void domain_ref(Domain *domain);

void domain_deref(Domain *domain);

void domain_add(Domain *self, Object *object);

void domain_remove(Domain *self, BrHandle handle);

ObjectLookupResult domain_resolve(Domain *self, BrHandle handle);

void object_init(Object *self, ObjectType type, ObjectDtor *dtor);

void object_ref(Object *self);

void object_deref(Object *self);

ObjectLookupResult global_lookup(BrHandle handle);
