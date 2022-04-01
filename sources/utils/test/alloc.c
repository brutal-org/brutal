#include <bruwutal/alloc.h>
#include "test/test.h"

HeapAlloc heap;

void test_alloc_begin_test(void)
{
    heap_alloc_init(&heap, NODE_DEFAULT);
}

void test_alloc_end_test(void)
{
    heap_alloc_deinit(&heap);
}

Alloc *test_alloc(void)
{
    return base$(&heap);
}
