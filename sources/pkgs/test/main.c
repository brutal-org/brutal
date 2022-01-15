#include <brutal/debug.h>
#include <brutal/task.h>
#include <brutal/text.h>
#include "test/test.h"

static size_t _len = 0;
static Test _tests[1024] = {};

void test_register(Test test)
{
    _tests[_len] = test;
    _len++;
}

bool test_run(Test test)
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
        bool pass = result == TASK_EXIT_SUCCESS;

        if (test.flags & TEST_EXPECTED_TO_FAIL)
        {
            pass = result != TASK_EXIT_SUCCESS;
        }

        if (pass)
        {
            log$("[ PASS ] {}", test.name);
        }
        else
        {
            log$("[ FAIL ] {}", test.name);
        }

        return pass;
    }
}

void test_run_by_pattern(Str pattern)
{
    for (size_t i = 0; i < _len; i++)
    {
        if (glob_match_str(pattern, str$(_tests[i].name)))
        {
            test_run(_tests[i]);
        }
    }
}

int test_run_all(void)
{
    int pass_count = 0;
    int fail_count = 0;

    log$("Running {} tests...", _len);
    log$("");

    for (size_t i = 0; i < _len; i++)
    {
        Test test = _tests[i];

        if (test_run(test))
        {
            pass_count++;
        }
        else
        {
            fail_count++;
        }
    }

    log$("");
    log$("{} tests run, {} pass, {} fail", _len, pass_count, fail_count);
    log$("");

    return fail_count != 0 ? TASK_EXIT_FAILURE : TASK_EXIT_SUCCESS;
}

int main(int argc, char const *argv[])
{
    if (argc == 3 && str_eq(str$(argv[1]), str$("-t")))
    {
        test_run_by_pattern(str$(argv[2]));
    }
    else
    {
        return test_run_all();
    }
}
