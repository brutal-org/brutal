#pragma once

#include <brutal/base.h>

typedef void test_func_t(void);

struct test
{
    str_t name;
    test_func_t *func;
};

void test_register(struct test test);

#define TEST(name)                                                                      \
    void test_##name(void);                                                             \
    __attribute__((constructor)) static inline void test_register_##name##_detail(void) \
    {                                                                                   \
        test_register((struct test){                                                    \
            make_str(name),                                                             \
            test_##name,                                                                \
        });                                                                             \
    }                                                                                   \
    void test_##name(void)
