#pragma once

#include "kernel/object.h"

typedef struct
{
    OBJECT_HEADER;

    Lock lock;
    VecObject objects;
} Domain;

Domain *domain_create(void);

void domain_ref(Domain *domain);

void domain_deref(Domain *domain);

void domain_add(Domain *self, Object *object);

void domain_remove(Domain *self, BrHandle handle);

Object *domain_lookup(Domain *self, BrHandle handle, ObjectType type);
