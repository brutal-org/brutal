#pragma once

#include <brutal/base.h>
#include <brutal/log.h>

typedef void TestFunc(void);

enum test_flags
{
    TEST_NONE = 0,
    TEST_EXPECTED_TO_FAIL = 1 << 0,
};

enum test_result
{
    TEST_PASS,
    TAST_FAIL,
};

struct test
{
    enum test_flags flags;
    Str name;
    TestFunc *func;
};

void test_register(struct test test);

#define TEST_FUNCTION_NAME(name) test_##name

#define TEST_WITH_FLAGS(name, flags)                                            \
    void TEST_FUNCTION_NAME(name)(void);                                        \
    [[gnu::constructor]] static inline void test_register_##name##_detail(void) \
    {                                                                           \
        test_register((struct test){                                            \
            flags,                                                              \
            str_cast(#name),                                                    \
            TEST_FUNCTION_NAME(name),                                           \
        });                                                                     \
    }                                                                           \
    void TEST_FUNCTION_NAME(name)(void)

#define TEST(name) \
    TEST_WITH_FLAGS(name, TEST_NONE)
