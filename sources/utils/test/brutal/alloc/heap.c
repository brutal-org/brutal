#include <brutal/alloc.h>
#include "test/test.h"

static HeapAlloc _heap;
static bool _init = false;

static Alloc *heap(void)
{

    if (!_init)
    {
        heap_alloc_init(&_heap, NODE_DEFAULT);
        _init = true;
    }

    return base$(&_heap);
}

TEST(heap)
{
    Alloc *alloc = heap();
    assert_not_null(alloc);
    assert_not_null(alloc->acquire);
    assert_not_null(alloc->release);
    assert_not_null(alloc->resize);
}

TEST(heap_malloc)
{
    void *v = alloc_malloc(heap(), 0x100);

    assert_not_null(v);
}

TEST(heap_free)
{
    void *v = alloc_malloc(heap(), 0x100);

    assert_not_null(v);

    alloc_free(heap(), v);
}

TEST(heap_calloc)
{
    uint8_t *v = (uint8_t *)alloc_calloc(heap(), 10, 10); // should return

    assert_not_null(v);
    for (size_t i = 0; i < 100; i++)
    {
        assert_equal(v[i], 0);
    }

    alloc_free(heap(), v);
}

TEST(heap_resize)
{

    uint8_t *v = (uint8_t *)alloc_malloc(heap(), 0x50);

    v = (uint8_t *)alloc_resize(heap(), v, 0x1000);

    assert_not_null(v);
    mem_set(v, 1, 0x1000);

    alloc_free(heap(), v);
}

TEST(heap_resize_lower)
{

    uint8_t *v = (uint8_t *)alloc_malloc(heap(), 0x1000);

    v = (uint8_t *)alloc_resize(heap(), v, 0x50);

    assert_not_null(v);

    alloc_free(heap(), v);
}

TEST(heap_malloc_zero)
{
    void *v = alloc_malloc(heap(), 0);

    assert_not_null(v);

    alloc_free(heap(), v);
}

TEST(heap_resize_zero)
{
    void *v = alloc_malloc(heap(), 0x100);

    assert_not_null(v);

    v = alloc_resize(heap(), v, 0);

    assert_equal(v, nullptr);
}

TEST(heap_free_null)
{
    alloc_free(heap(), nullptr);
}

TEST(heap_resize_null)
{
    void *v = alloc_resize(heap(), nullptr, 10);

    assert_not_null(v);

    alloc_free(heap(), v);
}

TEST_WITH_FLAGS(heap_free_invalid_addr, TEST_EXPECTED_TO_FAIL)
{
    uint8_t *v = (uint8_t *)alloc_malloc(heap(), 0x1000);

    assert_not_null(v);

    v = v + 0x100;

    alloc_free(heap(), v);
}

TEST_WITH_FLAGS(heap_malloc_too_big_size, TEST_EXPECTED_TO_FAIL)
{
    // it's around 1152921 petabytes so I don't think it should succeed
    uint8_t *v = (uint8_t *)alloc_malloc(heap(), 0x1000000000000000);

    (void)v;
}

// up to 44M of usage
TEST(heap_malloc_realloc_stress_test)
{
    void *v[512];

    for (size_t i = 2; i < 512; i++)
    {
        v[i] = alloc_malloc(heap(), i);
        assert_not_null(v[i]);
    }

    for (size_t i = 2; i < 512; i++)
    {
        v[i] = alloc_resize(heap(), v[i], i * i);
        assert_not_null(v[i]);
        mem_set(v[i], 0, i * i - 1);
    }

    for (size_t i = 2; i < 512; i++)
    {
        alloc_free(heap(), v[i]);
    }
}
