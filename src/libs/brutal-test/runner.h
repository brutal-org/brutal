#pragma once

#include <brutal-ds>

/* --- Tests ---------------------------------------------------------------- */

#define TEST_NONE (0)
#define TEST_EXPECTED_TO_FAIL (1 << 0)
#define TEST_DISABLED (1 << 1)

typedef uint64_t TestFlags;

typedef void TestFn(void);

typedef enum
{
    TEST_FAIL,
    TEST_PASS,
} TestState;

typedef struct
{
    TestFlags flags;
    Str name;
    Loc loc;
    TestFn *fn;

    TestState state;
} Test;

typedef struct
{
    Str name;
    Loc loc;
} TestCase;

/* --- Hooks ---------------------------------------------------------------- */

typedef void *TestHookCtor(void *args);
typedef void TestHookDtor(void *self);

typedef struct
{
    int ref;
    uint64_t id;
    void *data;
    TestHookDtor *dtor;
} TestHook;

typedef struct TestCtx
{
    uint64_t seed;
    Vec(TestHook) hooks;

    int counter_total;
    int counter_passed;
    int counter_failed;
    int counter_skipped;

    Test *current_test;
    TestCase *current_case;
} TestCtx;

void test_register(Test test);

TestCtx *test_self(void);

void test_init(TestCtx *self, Alloc *alloc);

void test_deinit(TestCtx *self);

void test_begin_suite(TestCtx *self);

bool test_end_suite(TestCtx *self);

void test_run_test(TestCtx *self, Test *test);

void test_run_all(TestCtx *self);

void test_run_sel(TestCtx *self, Str pattern);

bool test_begin_case(TestCtx *self, TestCase *test_case);

void test_end_case(TestCtx *self);

void test_progress(TestCtx *self);

void test_fail(TestCtx *self, Loc loc, Str msg, AnyVa args);

void test_expect(TestCtx *self, Loc loc, Any lhs, Any rhs, char const *op);

void *test_hook_use(TestCtx *self, uint64_t id, void *args, TestHookCtor *ctor, TestHookDtor *dtor);

void test_hook_ref(TestCtx *self, uint64_t id);

void test_hook_deref(TestCtx *self, uint64_t id);

#define test_case$(NAME)                  \
    static TestCase var$(__test_case) = { \
        .name = str_const$(#NAME),        \
        .loc = loc_const$(),              \
    };                                    \
    cond_defer$(test_begin_case(test_self(), &var$(__test_case)), test_end_case(test_self()))

#define test_fail$(MSG, ...) test_fail(test_self(), loc$(), str$(MSG), any_va$(__VA_ARGS__))

#define test_expect$(LHS, RHS, OP) test_expect(test_self(), loc$(), LHS, RHS, OP)

#define test_use$(ID, ARGS, CTOR, DTOR) test_use(test_self(), ID, ARGS, CTOR, DTOR)

#define __test_fn$(NAME) __test_##NAME

#define __test_empty_flag$(FLAG)
#define __test_flag$(FLAG) | FLAG

#define test$(NAME, ...)                                                           \
    void __test_fn$(NAME)(void);                                                   \
    [[gnu::constructor]] static void __test_##NAME##_register(void)                \
    {                                                                              \
        test_register((Test){                                                      \
            .flags = TEST_NONE MAP(__test_flag$, __VA_ARGS__) IFE(__VA_ARGS__)(0), \
            .name = str$(#NAME),                                                   \
            .loc = loc_const$(),                                                   \
            .fn = __test_fn$(NAME),                                                \
        });                                                                        \
    }                                                                              \
    void __test_fn$(NAME)(void)
