#include <brutal-ds>
#include <brutal-test>

test$(map_put_get)
{
    Map(uint32_t) map;
    uint32_t value;

    map_init(&map, test_use_alloc());

    map_put(&map, "test0", 0xfefefefe);
    map_put(&map, "test1", 0xfafafafa);
    map_put(&map, "test2", 0xDEADC0DE);
    map_put(&map, "test3", 0xC001C0DE);

    expect$(map_get(&map, "test0", &value));
    expect_equal$(value, 0xfefefefe);

    expect$(map_get(&map, "test1", &value));
    expect_equal$(value, 0xfafafafa);

    expect$(map_get(&map, "test2", &value));
    expect_equal$(value, 0xDEADC0DE);

    expect$(map_get(&map, "test3", &value));
    expect_equal$(value, 0xC001C0DE);
}
