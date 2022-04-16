#include <brutal-test>

test$(should_run)
{
}

test$(should_fail, TEST_EXPECTED_TO_FAIL)
{
    test_fail$("this test should fail");
}
