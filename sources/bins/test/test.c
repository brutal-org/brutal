#include <brutal/debug.h>
#include <brutal/task.h>
#include "test/test.h"

static size_t tests_count = 0;
static struct test tests[1024] = {};

void test_register(struct test test)
{
    tests[tests_count] = test;
    tests_count++;
}

bool test_run(struct test test)
{
    struct task runner;
    task_fork(&runner);
    task_run(&runner);

    if (task_is_child(&runner))
    {
        test_alloc_begin_test();
        test.func();
        test_alloc_end_test();
        task_exit(&runner, TASK_EXIT_SUCCESS);
        panic$("tak_exit returned")
    }
    else
    {

        int result = UNWRAP(task_wait(&runner));

        if (test.flags & TEST_EXPECTED_TO_FAIL)
        {
            return result != TASK_EXIT_SUCCESS;
        }
        else
        {
            return result == TASK_EXIT_SUCCESS;
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

    log$("Running {} tests...", tests_count);
    log$("");

    for (size_t i = 0; i < tests_count; i++)
    {
        struct test test = tests[i];

        if (test_run(test))
        {
            log$("[ PASS ] {}", test.name);
            pass_count++;
        }
        else
        {
            log$("[ FAIL ] {}", test.name);
            fail_count++;
        }
    }

    log$("");
    log$("{} tests run, {} pass, {} fail", tests_count, pass_count, fail_count);
    log$("");

    return fail_count != 0 ? TASK_EXIT_FAILURE : TASK_EXIT_SUCCESS;
}

int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char const *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        log$("argc[{}]='{}'", i, str$(argv[i]));
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
