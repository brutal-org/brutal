#include <brutal/base/endian.h>
#include "tests/test.h"

TEST(endian_load_store_be)
{
    be_uint32_t value;
    store_be(69420, &value);
    uint32_t result = load_be(value);
    assert_equal(result, 69420u);
}

TEST(endian_load_store_le)
{
    le_uint32_t value;
    store_le(69420, &value);
    uint32_t result = load_le(value);
    assert_equal(result, 69420u);
}
