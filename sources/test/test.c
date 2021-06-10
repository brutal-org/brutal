#include <brutal/log.h>
#include <brutal/task.h>
#include "test/test.h"

static size_t tests_count = 0;
static struct test tests[1024] = {0};

void test_register(struct test test)
{
    tests[tests_count] = test;
    tests_count++;
}

void test_run(struct test test)
{
    struct task runner;
    task_fork(&runner);
    log("Running {}...", test.name);
    task_run(&runner);

    if (task_child(&runner))
    {
        test.func();
        task_exit(&runner, TASK_EXIT_SUCCESS);
    }
    else
    {
        task_wait(&runner);
    }
}

void test_run_all(void)
{
    log("Running {} tests...", tests_count);

    for (size_t i = 0; i < tests_count; i++)
    {
        test_run(tests[i]);
    }
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    test_run_all();
    return 0;
}
