#include <library/task.h>

#include "test/test.h"

static size_t test_count = 0;
static struct test tests[1024] = {0};

void test_register(struct test test)
{
    tests[test_count] = test;
    test_count++;
}

void test_run(struct test test)
{
    struct task runner;
    task_fork(&runner);

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
    for (size_t i = 0; i < test_count; i++)
    {
        test_run(tests[i]);
    }
}

int main(int argc, char const *argv[])
{
    test_run_all();
    return 0;
}
