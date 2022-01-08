#include <brutal/alloc.h>
#include <brutal/ds.h>
#include "test/test.h"

TEST(vec)
{
    Vec(int) vec;

    vec_init(&vec, alloc_global());
    vec_deinit(&vec);
}

TEST(vec_push)
{
    Vec(int) vec;

    vec_init(&vec, alloc_global());
    vec_push(&vec, 10);

    assert_equal(vec.data[0], 10);
    assert_equal(vec.len, 1);
    vec_deinit(&vec);
}

TEST(vec_foreach_indexed_test)
{
    Vec(int) vec;

    vec_init(&vec, alloc_global());

    for (int i = 0; i < 100; i++)
    {
        vec_push(&vec, i);
    }

    int i2 = 0;
    vec_foreach_idx(i, v, &vec)
    {
        assert_equal(i, i2);
        assert_equal(*v, i2);
        i2++;
    }

    vec_deinit(&vec);
}
