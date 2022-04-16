#pragma once

#include <bal/abi.h>
#include <bal/boot.h>
#include <brutal-ds>

#define PMM_USED (true)
#define PMM_UNUSED (false)

typedef USizeRange PmmRange;

typedef Result(BrResult, PmmRange) PmmResult;

void pmm_initialize(Handover const *handover);

PmmResult pmm_alloc(size_t size, bool upper);

PmmResult pmm_used(PmmRange range);

PmmResult pmm_unused(PmmRange range);
