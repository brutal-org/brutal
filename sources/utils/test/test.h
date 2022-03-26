#pragma once

#include <brutal/debug.h>
#include <brutal/gfx.h>

typedef void TestFunc(void);

enum test_flags
{
    TEST_NONE = 0,
    TEST_EXPECTED_TO_FAIL = 1 << 0,
    TEST_GFX = 1 << 1,
    TEST_COUNTED = 1 << 2,
};

typedef struct
{
    enum test_flags flags;
    Str name;
    TestFunc *func;

    int time_count;
} Test;

void test_alloc_begin_test(void);

void test_alloc_end_test(void);

Alloc *test_alloc(void);

Gfx *test_gfx(void);

void _test_set_iteration(int t);

int test_iteration(void);

void test_register(Test test);

double test_gfx_run(Test test, long default_test_runtime);

#define TEST_FUNCTION_NAME(name) test_##name

#define TEST_WITH_FLAGS_COUNT(name, flags, count)                               \
    void TEST_FUNCTION_NAME(name)(void);                                        \
    [[gnu::constructor]] static inline void test_register_##name##_detail(void) \
    {                                                                           \
        test_register((Test){                                                   \
            flags,                                                              \
            str$(#name),                                                        \
            TEST_FUNCTION_NAME(name),                                           \
            (count),                                                            \
        });                                                                     \
    }                                                                           \
    void TEST_FUNCTION_NAME(name)(void)

#define TEST_WITH_FLAGS(name, flags) \
    TEST_WITH_FLAGS_COUNT(name, (flags), 1)

#define TEST(name) \
    TEST_WITH_FLAGS_COUNT(name, TEST_NONE, 0)

#define GFX_TEST(name) \
    TEST_WITH_FLAGS_COUNT(name, TEST_GFX, 0)

#define GFX_TEST_COUNT(name, amount) \
    TEST_WITH_FLAGS_COUNT(name, (TEST_GFX | TEST_COUNTED), (amount))
