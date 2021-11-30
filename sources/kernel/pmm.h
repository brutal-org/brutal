#pragma once

#include <bal/abi.h>
#include <bal/boot.h>
#include <brutal/ds/bits.h>

#define PMM_USED (true)
#define PMM_UNUSED (false)

typedef USizeRange PmmRange;

typedef Result(BrResult, PmmRange) PmmResult;

// don't put void, because some implementations may uses different arguments
void pmm_initialize();

PmmResult pmm_alloc(size_t size, bool upper);

PmmResult pmm_used(PmmRange range);

PmmResult pmm_unused(PmmRange range);
