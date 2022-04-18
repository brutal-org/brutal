#include <brutal-test>

test$(endian_be_load$_store$16)
{
    be_uint16_t value;
    be_store$(0x1a2b, &value);
    uint16_t result = be_load$(value);
    expect_equal$(result, (uint16_t)0x1a2b);
    expect_equal$(value._be_data, (uint16_t)0x2b1a);
}

test$(endian_le_load$_store$16)
{
    le_uint16_t value;
    le_store$(0x1a2b, &value);
    uint16_t result = le_load$(value);
    expect_equal$(result, (uint16_t)0x1a2b);
}

test$(endian_be_load$_store$32)
{
    be_uint32_t value;
    be_store$(0x1a2b3c4d, &value);
    uint32_t result = be_load$(value);
    expect_equal$(result, (uint32_t)0x1a2b3c4d);
}

test$(endian_le_load$_store$32)
{
    le_uint32_t value;
    le_store$(0x1a2b3c4d, &value);
    uint32_t result = le_load$(value);
    expect_equal$(result, (uint32_t)0x1a2b3c4d);
}
