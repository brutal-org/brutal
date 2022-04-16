#include <brutal-ds>
#include <brutal-test>

test$(vec_init_deinit)
{
    Vec(int) vec;

    vec_init(&vec, test_use_alloc());
    vec_deinit(&vec);
}

test$(vec_push)
{
    Vec(int) vec;
    vec_init(&vec, test_use_alloc());
    vec_push(&vec, 10);

    expect_equal$(vec.data[0], 10);
    expect_equal$(vec.len, 1);
    vec_deinit(&vec);
}

test$(vec_foreach_indexed_test)
{
    Vec(int) vec;
    vec_init(&vec, test_use_alloc());

    for (int i = 0; i < 100; i++)
    {
        vec_push(&vec, i);
    }

    int i2 = 0;
    vec_foreach_idx(i, v, &vec)
    {
        expect_equal$(i, i2);
        expect_equal$(*v, i2);
        i2++;
    }

    vec_deinit(&vec);
}
