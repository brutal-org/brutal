#pragma once

#include <brutal/base.h>
#include "kernel/handover.h"
#include "syscalls/error.h"

#define PMM_USED (true)
#define PMM_UNUSED (false)

typedef Range(size_t) PmmRange;

typedef Result(BrError, PmmRange) pmm_Result;

void pmm_initialize(struct handover const *handover);

pmm_Result pmm_alloc(size_t size);

pmm_Result pmm_used(PmmRange range);

pmm_Result pmm_unused(PmmRange range);
