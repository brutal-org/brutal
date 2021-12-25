#pragma once

#include "kernel/object.h"

typedef struct
{
    OBJECT_HEADER;

    RwLock lock;
    SlotObject objects;
} Domain;

Domain *domain_create(void);

void domain_ref(Domain *domain);

void domain_deref(Domain *domain);

BrHandle domain_add(Domain *self, Object *object);

BrResult domain_remove(Domain *self, BrHandle handle);

Object *domain_lookup(Domain *self, BrHandle handle, BrType type);
