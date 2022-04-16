#include <brutal-test>

int main(int argc, char const *argv[])
{
    TestCtx ctx = {};

    test_init(&ctx, alloc_global());
    test_begin_suite(&ctx);

    if (argc == 3 && str_eq(str$(argv[1]), str$("-t")))
    {
        test_run_sel(&ctx, str$(argv[2]));
    }
    else
    {
        test_run_all(&ctx);
    }

    bool success = test_end_suite(&ctx);
    test_deinit(&ctx);

    return success ? 0 : 1;
}
