#include <brutal/math/fast_math.h>
#include <math.h>
#include "test/test.h"

TEST(fast_sqrt_accuracy)
{
    for (float i = 0; i < 100; i += 0.1f)
    {
        float expected = sqrt(i);
        float got = fast_sqrt(i);

        assert_greater_than(got, expected - 0.01f);
        assert_lower_than(got, expected + 0.01f);
    }
}

TEST(fast_invert_sqrt_accuracy)
{
    for (float i = 0.1; i < 100; i += 0.1f)
    {
        float expected = 1 / sqrt(i);
        float got = fast_invert_sqrt(i);

        assert_greater_than(got, expected - 0.01f);
        assert_lower_than(got, expected + 0.01f);
    }
}

TEST(fast_acos_accuracy)
{
    for (float i = -1; i < 1; i += 0.1f)
    {
        float expected = acosf(i);
        float got = fast_acos(i);

        assert_greater_than(got, expected - 0.2f);
        assert_lower_than(got, expected + 0.2f);
    }
}

TEST(fast_atan2_accuracy)
{
    for (float i = -(50); i < (50); i += 0.1f)
    {
        for (float i2 = -(50); i2 < (50); i2 += 0.1f)
        {
            float expected = atan2f(i, i2);
            float got = fast_atan2(i, i2);

            assert_greater_than(got, expected - 0.02f);
            assert_lower_than(got, expected + 0.02f);
        }
    }
}
