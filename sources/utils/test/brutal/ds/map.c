#include <brutal/alloc.h>
#include <brutal/ds.h>
#include "test/test.h"

TEST(map_put_get)
{
    Map(uint32_t) map;
    uint32_t value;

    map_init(&map, test_alloc());

    map_put(&map, "test0", 0xfefefefe);
    map_put(&map, "test1", 0xfafafafa);
    map_put(&map, "test2", 0xDEADC0DE);
    map_put(&map, "test3", 0xC001C0DE);

    assert_truth(map_get(&map, "test0", &value));
    assert_equal(value, 0xfefefefe);

    assert_truth(map_get(&map, "test1", &value));
    assert_equal(value, 0xfafafafa);

    assert_truth(map_get(&map, "test2", &value));
    assert_equal(value, 0xDEADC0DE);

    assert_truth(map_get(&map, "test3", &value));
    assert_equal(value, 0xC001C0DE);
}
