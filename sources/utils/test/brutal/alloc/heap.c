#include <brutal-test>

test$(heap_traits)
{
    Alloc *alloc = test_use_alloc();
    expect_not_null$(alloc);
    expect_not_null$(alloc->acquire);
    expect_not_null$(alloc->release);
    expect_not_null$(alloc->resize);
}

test$(heap_malloc)
{
    void *v = alloc_malloc(test_use_alloc(), 0x100);
    expect_not_null$(v);
}

test$(heap_free)
{
    void *v = alloc_malloc(test_use_alloc(), 0x100);
    expect_not_null$(v);
    alloc_free(test_use_alloc(), v);
}

test$(heap_calloc)
{
    uint8_t *v = (uint8_t *)alloc_calloc(test_use_alloc(), 10, 10); // should return

    expect_not_null$(v);
    for (size_t i = 0; i < 100; i++)
    {
        expect_equal$(v[i], 0);
    }

    alloc_free(test_use_alloc(), v);
}

test$(heap_resize)
{
    uint8_t *v = (uint8_t *)alloc_malloc(test_use_alloc(), 0x50);

    v = (uint8_t *)alloc_resize(test_use_alloc(), v, 0x1000);

    expect_not_null$(v);
    mem_set(v, 1, 0x1000);

    alloc_free(test_use_alloc(), v);
}

test$(heap_resize_lower)
{

    uint8_t *v = (uint8_t *)alloc_malloc(test_use_alloc(), 0x1000);

    v = (uint8_t *)alloc_resize(test_use_alloc(), v, 0x50);

    expect_not_null$(v);

    alloc_free(test_use_alloc(), v);
}

test$(heap_malloc_zero)
{
    void *v = alloc_malloc(test_use_alloc(), 0);

    expect_not_null$(v);

    alloc_free(test_use_alloc(), v);
}

test$(heap_resize_zero)
{
    void *v = alloc_malloc(test_use_alloc(), 0x100);

    expect_not_null$(v);

    v = alloc_resize(test_use_alloc(), v, 0);

    expect_null$(v);
}

test$(heap_free_null)
{
    alloc_free(test_use_alloc(), nullptr);
}

test$(heap_resize_null)
{
    void *v = alloc_resize(test_use_alloc(), nullptr, 10);

    expect_not_null$(v);

    alloc_free(test_use_alloc(), v);
}

test$(heap_malloc_realloc_stress_test)
{
    // up to 44M of usage
    void *v[512];

    for (size_t i = 2; i < 512; i++)
    {
        v[i] = alloc_malloc(test_use_alloc(), i);
        expect_not_null$(v[i]);
    }

    for (size_t i = 2; i < 512; i++)
    {
        v[i] = alloc_resize(test_use_alloc(), v[i], i * i);
        expect_not_null$(v[i]);
        mem_set(v[i], 0, i * i - 1);
    }

    for (size_t i = 2; i < 512; i++)
    {
        alloc_free(test_use_alloc(), v[i]);
    }
}
