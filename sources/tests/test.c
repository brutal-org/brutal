#include <brutal/log.h>
#include <brutal/task.h>
#include "tests/test.h"

static size_t tests_count = 0;
static struct test tests[1024] = {};

void test_register(struct test test)
{
    tests[tests_count] = test;
    tests_count++;
}

enum test_result test_run(struct test test)
{
    struct task runner;
    task_fork(&runner);
    task_run(&runner);

    if (task_child(&runner))
    {
        test.func();
        task_exit(&runner, TASK_EXIT_SUCCESS);
        assert_unreachable();
    }
    else
    {
        auto result = UNWRAP(task_wait(&runner));

        if (result == TASK_EXIT_SUCCESS)
        {
            return TEST_PASS;
        }
        else
        {
            if (test.flags & TEST_EXPECTED_TO_FAIL)
            {
                return TEST_PASS;
            }
            else
            {
                return TAST_FAIL;
            }
        }
    }
}

void test_run_by_name(Str name)
{
    for (size_t i = 0; i < tests_count; i++)
    {
        if (str_eq(tests[i].name, name))
        {
            tests[i].func();
        }
    }
}

int test_run_all(void)
{
    int pass_count = 0;
    int fail_count = 0;

    log("Running {} tests...", tests_count);
    log("");

    for (size_t i = 0; i < tests_count; i++)
    {
        auto test = tests[i];

        auto result = test_run(test);

        if (result == TEST_PASS)
        {
            log("[ PASS ] {}", test.name);
            pass_count++;
        }
        else
        {
            log("[ FAIL ] {}", test.name);
            fail_count++;
        }
    }

    log("");
    log("{} tests run, {} pass, {} fail", tests_count, pass_count, fail_count);
    log("");

    if (fail_count != 0)
    {
        return TASK_EXIT_FAILURE;
    }
    else
    {
        return TASK_EXIT_SUCCESS;
    }
}

TEST(should_run)
{
}

TEST_WITH_FLAGS(should_fail, TEST_EXPECTED_TO_FAIL)
{
    panic("this test should fail");
}

int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char const *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        log("argc[{}]='{}'", i, str$(argv[i]));
    }

    if (argc == 3 && str_eq(str$(argv[1]), str$("-t")))
    {
        test_run_by_name(str$(argv[2]));
    }
    else
    {
        return test_run_all();
    }
}
