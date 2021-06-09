#pragma once

#include <library/base.h>
#include "syscalls/error.h"

typedef range_t(size_t) heap_range_t;

typedef result_t(br_error_t, heap_range_t) heap_result_t;

heap_result_t heap_alloc(size_t size);

heap_result_t heap_free(heap_range_t range);
