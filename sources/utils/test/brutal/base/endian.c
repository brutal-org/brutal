#include <brutal/tests.h>

test$(endian_load_store_be16)
{
    be_uint16_t value;
    store_be(0x1a2b, &value);
    uint16_t result = load_be(value);
    expect_equal$(result, (uint16_t)0x1a2b);
    expect_equal$(value._be_data, (uint16_t)0x2b1a);
}

test$(endian_load_store_le16)
{
    le_uint16_t value;
    store_le(0x1a2b, &value);
    uint16_t result = load_le(value);
    expect_equal$(result, (uint16_t)0x1a2b);
}

test$(endian_load_store_be32)
{
    be_uint32_t value;
    store_be(0x1a2b3c4d, &value);
    uint32_t result = load_be(value);
    expect_equal$(result, (uint32_t)0x1a2b3c4d);
}

test$(endian_load_store_le32)
{
    le_uint32_t value;
    store_le(0x1a2b3c4d, &value);
    uint32_t result = load_le(value);
    expect_equal$(result, (uint32_t)0x1a2b3c4d);
}
