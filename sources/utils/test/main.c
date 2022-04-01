#include <brutal/debug.h>
#include <brutal/gfx.h>
#include <brutal/parse.h>
#include <brutal/task.h>
#include <brutal/text.h>
#include "test/test.h"

static size_t _len = 0;
static Test _tests[1024] = {};
static int t = 0;
static long default_test_runtime = 3;

void test_register(Test test)
{
    _tests[_len] = test;
    _len++;
}

int test_iteration(void)
{
    return t;
}

void _test_set_iteration(int v)
{
    t = v;
}

bool test_run(Test test)
{
    struct _Task runner;
    task_fork(&runner);
    task_run(&runner);

    if (task_is_child(&runner))
    {
        test_alloc_begin_test();

        if (test.flags & TEST_GFX)
        {
            test_gfx_run(test, default_test_runtime);
        }
        else
        {
            if (test.flags & TEST_COUNTED)
            {
                for (t = 0; t < test.time_count; t++)
                {
                    test.func();
                }
            }
            else
            {
                test.func();
            }
        }

        test_alloc_end_test();
        task_exit(&runner, TASK_EXIT_SUCCESS);
        panic$("tak_exit returned")
    }

    int result = UNWRAP(task_wait(&runner));
    bool pass = result == TASK_EXIT_SUCCESS;

    if (test.flags & TEST_EXPECTED_TO_FAIL)
    {
        pass = result != TASK_EXIT_SUCCESS;
    }

    if (pass)
    {
        log$("[ {fg-green} ] {}", "PASS", test.name);
    }
    else
    {
        log$("[ {fg-red} ] {} ({})", "FAIL", test.name, result);
    }

    return pass;
}

int test_run_by_pattern(Str pattern)
{
    int pass_count = 0;
    int fail_count = 0;

    for (size_t i = 0; i < _len; i++)
    {
        if (glob_match_str(pattern, str$(_tests[i].name)))
        {
            if (test_run(_tests[i]))
            {
                pass_count++;
            }
            else
            {
                fail_count++;
            }
        }
    }

    return fail_count != 0 ? TASK_EXIT_FAILURE : TASK_EXIT_SUCCESS;
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
    Str patter_to_run = str$("");

    // FIXME: implement a good way to parse arguments in the brutal library

    for (int i = 1; i < argc; i++)
    {
        if (str_eq(str$(argv[i]), str$("-t")))
        {
            assert_lower_than(i + 1, argc);
            patter_to_run = str$(argv[i + 1]);
            i++;
        }
        else if (str_eq(str$(argv[i]), str$("-l")))
        {
            log$("dumping different tests:");

            for (size_t y = 0; y < _len; y++)
            {
                log$("- {}", _tests[y].name);
            }

            return 0;
        }
        else if (str_eq(str$(argv[i]), str$("-s")))
        {
            assert_lower_than(i + 1, argc);
            default_test_runtime = 0;
            str_to_int(str$(argv[i + 1]), &default_test_runtime);
            i++;
        }
    }

    if (patter_to_run.len != 0)
    {
        return test_run_by_pattern(patter_to_run);
    }
    else
    {
        return test_run_all();
    }
}
