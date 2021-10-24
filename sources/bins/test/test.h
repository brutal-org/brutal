#pragma once

#include <brutal/debug.h>

typedef void TestFunc(void);

enum test_flags
{
    TEST_NONE = 0,
    TEST_EXPECTED_TO_FAIL = 1 << 0,
};

struct test
{
    enum test_flags flags;
    Str name;
    TestFunc *func;
};

void test_alloc_begin_test(void);

void test_alloc_end_test(void);

Alloc *test_alloc(void);

void test_register(struct test test);

#define TEST_FUNCTION_NAME(name) test_##name

#define TEST_WITH_FLAGS(name, flags)                                            \
    void TEST_FUNCTION_NAME(name)(void);                                        \
    [[gnu::constructor]] static inline void test_register_##name##_detail(void) \
    {                                                                           \
        test_register((struct test){                                            \
            flags,                                                              \
            str$(#name),                                                        \
            TEST_FUNCTION_NAME(name),                                           \
        });                                                                     \
    }                                                                           \
    void TEST_FUNCTION_NAME(name)(void)

#define TEST(name) \
    TEST_WITH_FLAGS(name, TEST_NONE)
