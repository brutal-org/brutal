#pragma once

#include <brutal/ds.h>
#include <brutal/sync.h>
#include "arch/heap.h"
#include "arch/pmm.h"
#include "arch/vmm.h"

typedef struct
{
    refcount_t refcount;

    pmm_range_t range;
} memory_object_t;

typedef struct
{
    memory_object_t *object;
} memory_mapping_t;

typedef struct
{
    refcount_t refcount;

    vec_t(memory_mapping_t) mappings;
} memory_space_t;
