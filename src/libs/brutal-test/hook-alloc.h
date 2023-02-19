#pragma once

#include <brutal-alloc>
#include <brutal-debug>

#include "runner.h"

/* --- Alloc Hook ----------------------------------------------------------- */

#define TEST_ALLOC_HOOK (0x6db7d2746425cc3f)

static void *__test_use_alloc_ctor(void *)
{
    HeapAlloc *alloc = alloc_make(alloc_global(), HeapAlloc);
    heap_alloc_init(alloc, NODE_DEFAULT);
    return alloc;
}

static void __test_use_alloc_dtor(void *alloc)
{
    heap_alloc_deinit(alloc);
    alloc_free(alloc_global(), alloc);
}

MAYBE_UNUSED static inline Alloc *test_use_alloc(void)
{
    return test_hook_use(
        test_self(),
        TEST_ALLOC_HOOK,
        nullptr,
        __test_use_alloc_ctor,
        __test_use_alloc_dtor);
}
