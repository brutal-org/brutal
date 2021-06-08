#pragma once

#include <library/base.h>

#include "kernel/handover.h"
#include "syscalls/error.h"

#define PMM_USED (true)
#define PMM_UNUSED (false)

typedef range_t(size_t) pmm_range_t;

typedef result_t(br_error_t, pmm_range_t) pmm_result_t;

void pmm_initialize(struct handover const *handover);

pmm_result_t pmm_alloc(size_t size);

pmm_result_t pmm_used(pmm_range_t range);

pmm_result_t pmm_unused(pmm_range_t range);
