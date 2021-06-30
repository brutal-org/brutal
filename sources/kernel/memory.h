#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include "arch/heap.h"
#include "arch/pmm.h"
#include "arch/vmm.h"

struct memory_object
{
    RefCount refcount;

    PmmRange range;
};

struct memory_mapping
{
    struct memory_object *object;
};

struct memory_space
{
    RefCount refcount;

    Vec(struct memory_mapping) mappings;
};
