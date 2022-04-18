#include <brutal-test>

test$(expect_not_null_pass)
{
    expect_not_null$((void *)0x1234);
}

test$(expect_not_null_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_not_null$(nullptr);
}

test$(expect_truth_pass)
{
    expect_truth$(true);
}

test$(expect_truth_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_truth$(false);
}

test$(expect_falsity_pass)
{
    expect_falsity$(false);
}

test$(expect_falsity_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_falsity$(true);
}

test$(expect_equal_pass)
{
    expect_equal$(10, 10);
}

test$(expect_equal_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_equal$(10, 69);
}

test$(expect_not_equal_pass)
{
    expect_not_equal$(72, 42);
}

test$(expect_not_equal_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_not_equal$(0x69, 0x69);
}

test$(expect_greater_equal_pass)
{
    expect_greater_equal$(72, 72);
    expect_greater_equal$(96, 72);
}

test$(expect_greater_equal_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_greater_equal$(1, 72);
}

test$(expect_greater_than_pass)
{
    expect_greater_than$(96, 72);
}

test$(expect_greater_than_should_fail_if_equal, TEST_EXPECTED_TO_FAIL)
{
    expect_greater_than$(72, 72);
}

test$(expect_greater_than_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_greater_than$(1, 72);
}

test$(expect_lower_equal_pass)
{
    expect_lower_equal$(72, 72);
    expect_lower_equal$(1, 72);
}

test$(expect_lower_equal_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_lower_equal$(96, 72);
}

test$(expect_lower_than_pass)
{
    expect_lower_than$(1, 72);
}

test$(expect_lower_than_should_fail_if_equal, TEST_EXPECTED_TO_FAIL)
{
    expect_lower_than$(72, 72);
}

test$(expect_lower_than_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_lower_than$(96, 72);
}

test$(expect_equal_str_pass)
{
    expect_str_equal$(str$("foo"), str$("foo"));
}

test$(expect_equal_str_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_str_equal$(str$("fooowo"), str$("foo"));
}

test$(expect_not_equal_str_pass)
{
    expect_str_not_equal$(str$("foo"), str$("uwu"));
}

test$(expect_not_equal_str_should_fail, TEST_EXPECTED_TO_FAIL)
{
    expect_str_not_equal$(str$("foo"), str$("foo"));
}
