#include "test/test.h"

TEST(should_run)
{
}

TEST_WITH_FLAGS(should_fail, TEST_EXPECTED_TO_FAIL)
{
    panic$("this test should fail");
}
