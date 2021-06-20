#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include "arch/heap.h"
#include "arch/pmm.h"
#include "arch/vmm.h"

struct memory_object
{
    refcount_t refcount;

    pmm_range_t range;
};

struct memory_mapping
{
    struct memory_object *object;
};

struct memory_space
{
    refcount_t refcount;

    vec_t(struct memory_mapping) mappings;
};
