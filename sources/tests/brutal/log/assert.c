#include "tests/test.h"

TEST(assert_not_null_pass)
{
    assert_not_null((void *)0x1234);
}

TEST_WITH_FLAGS(assert_not_null_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_not_null(nullptr);
}

TEST(assert_truth_pass)
{
    assert_truth(true);
}

TEST_WITH_FLAGS(assert_truth_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_truth(false);
}

TEST(assert_falsity_pass)
{
    assert_falsity(false);
}

TEST_WITH_FLAGS(assert_falsity_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_falsity(true);
}

TEST(assert_equal_pass)
{
    assert_equal(10, 10);
}

TEST_WITH_FLAGS(assert_equal_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_equal(10, 69);
}

TEST(assert_not_equal_pass)
{
    assert_not_equal(72, 42);
}

TEST_WITH_FLAGS(assert_not_equal_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_not_equal(0x69, 0x69);
}

TEST(assert_greater_equal_pass)
{
    assert_greater_equal(72, 72);
    assert_greater_equal(96, 72);
}

TEST_WITH_FLAGS(assert_greater_equal_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_greater_equal(1, 72);
}

TEST(assert_greater_than_pass)
{
    assert_greater_than(96, 72);
}

TEST_WITH_FLAGS(assert_greater_than_should_fail_if_equal, TEST_EXPECTED_TO_FAIL)
{
    assert_greater_than(72, 72);
}

TEST_WITH_FLAGS(assert_greater_than_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_greater_than(1, 72);
}

TEST(assert_lower_equal_pass)
{
    assert_lower_equal(72, 72);
    assert_lower_equal(1, 72);
}

TEST_WITH_FLAGS(assert_lower_equal_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_lower_equal(96, 72);
}

TEST(assert_lower_than_pass)
{
    assert_lower_than(1, 72);
}

TEST_WITH_FLAGS(assert_lower_than_should_fail_if_equal, TEST_EXPECTED_TO_FAIL)
{
    assert_lower_than(72, 72);
}

TEST_WITH_FLAGS(assert_lower_than_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_lower_than(96, 72);
}

TEST_WITH_FLAGS(assert_unreachable_should_fail, TEST_EXPECTED_TO_FAIL)
{
    assert_unreachable();
}
