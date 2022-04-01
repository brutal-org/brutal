#include "tests/Driver.h"

TEST(assert_not_null)
{
    Assert::not_null((void *)0x1234);
}

TESTF(assert_not_null_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::not_null(nullptr);
}

TEST(assert_truth)
{
    Assert::truth(true);
}
TESTF(assert_truth_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::truth(false);
}

TEST(assert_falsity)
{
    Assert::falsity(false);
}
TESTF(assert_falsity_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::falsity(true);
}

TEST(assert_equal)
{
    Assert::equal(10, 10);
}
TESTF(assert_equal_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::equal(10, 69);
}

TEST(assert_not_equal)
{
    Assert::not_equal(72, 42);
}
TESTF(assert_not_equal_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::not_equal(0x69, 0x69);
}

TEST(assert_greater_equal)
{
    Assert::greater_equal(72, 72);
    Assert::greater_equal(96, 72);
}

TESTF(assert_greater_equal_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::greater_equal(1, 72);
}

TEST(assert_greater_than)
{
    Assert::greater_than(96, 72);
}

TESTF(assert_greater_than_should_fail_if_equal, Test::EXPECTED_TO_FAIL)
{
    Assert::greater_than(72, 72);
}

TESTF(assert_greater_than_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::greater_than(1, 72);
}

TEST(assert_lower_equal)
{
    Assert::lower_equal(72, 72);
    Assert::lower_equal(1, 72);
}

TESTF(assert_lower_equal_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::lower_equal(96, 72);
}

TEST(assert_lower_than)
{
    Assert::lower_than(1, 72);
}

TESTF(assert_lower_than_should_fail_if_equal, Test::EXPECTED_TO_FAIL)
{
    Assert::lower_than(72, 72);
}

TESTF(assert_lower_than_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::lower_than(96, 72);
}

TESTF(assert_unreachable_should_fail, Test::EXPECTED_TO_FAIL)
{
    Assert::unreachable();
}
