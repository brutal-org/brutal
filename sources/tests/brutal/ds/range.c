#include <brutal/alloc.h>
#include <brutal/ds.h>
#include "tests/test.h"

TEST(ds_range_alloc_used_start)
{
    RangeAlloc alloc;
    range_alloc_init(&alloc, alloc_global());

    range_alloc_unused(&alloc, (USizeRange){0x0, 0x5000});
    range_alloc_used(&alloc, (USizeRange){0x0, 0x1000});

    auto higher_half = (USizeRange){0x1000, 0x4000};

    log$("After test");
    range_alloc_dump(&alloc);

    assert_equal(alloc.length, 1);
    assert_truth(range_eq(alloc.data[0], higher_half));

    range_alloc_deinit(&alloc);
}

TEST(ds_range_alloc_used_center)
{
    RangeAlloc alloc;
    range_alloc_init(&alloc, alloc_global());

    range_alloc_unused(&alloc, (USizeRange){0x0, 0x5000});
    range_alloc_used(&alloc, (USizeRange){0x2000, 0x1000});

    auto lower_half = (USizeRange){0x0, 0x2000};
    auto higher_half = (USizeRange){0x3000, 0x2000};

    log$("After test");
    range_alloc_dump(&alloc);

    assert_equal(alloc.length, 2);
    assert_truth(range_eq(alloc.data[0], lower_half));
    assert_truth(range_eq(alloc.data[1], higher_half));

    range_alloc_deinit(&alloc);
}

TEST(ds_range_alloc_used_end)
{
    RangeAlloc alloc;
    range_alloc_init(&alloc, alloc_global());

    range_alloc_unused(&alloc, (USizeRange){0x0, 0x5000});
    range_alloc_used(&alloc, (USizeRange){0x4000, 0x1000});

    auto lower_half = (USizeRange){0x0, 0x4000};

    log$("After test");
    range_alloc_dump(&alloc);

    assert_equal(alloc.length, 1);
    assert_truth(range_eq(alloc.data[0], lower_half));

    range_alloc_deinit(&alloc);
}

TEST(ds_range_alloc_unused_begin)
{
    RangeAlloc alloc;
    range_alloc_init(&alloc, alloc_global());

    range_alloc_unused(&alloc, (USizeRange){0x0, 0x1000});
    range_alloc_unused(&alloc, (USizeRange){0x1000, 0x1000});

    auto final_range = (USizeRange){0x0, 0x2000};

    log$("After test");
    range_alloc_dump(&alloc);

    assert_equal(alloc.length, 1);
    assert_truth(range_eq(alloc.data[0], final_range));

    range_alloc_deinit(&alloc);
}

TEST(ds_range_alloc_unused_middle)
{
    RangeAlloc alloc;
    range_alloc_init(&alloc, alloc_global());

    range_alloc_unused(&alloc, (USizeRange){0x0, 0x1000});
    range_alloc_unused(&alloc, (USizeRange){0x2000, 0x1000});
    range_alloc_unused(&alloc, (USizeRange){0x1000, 0x1000});

    auto final_range = (USizeRange){0x0, 0x3000};

    log$("After test");
    range_alloc_dump(&alloc);

    assert_equal(alloc.length, 1);
    assert_truth(range_eq(alloc.data[0], final_range));

    range_alloc_deinit(&alloc);
}

TEST(ds_range_alloc_unused_end)
{
    RangeAlloc alloc;
    range_alloc_init(&alloc, alloc_global());

    range_alloc_unused(&alloc, (USizeRange){0x0, 0x1000});
    range_alloc_unused(&alloc, (USizeRange){0x1000, 0x1000});
    range_alloc_unused(&alloc, (USizeRange){0x2000, 0x1000});

    auto final_range = (USizeRange){0x0, 0x3000};

    log$("After test");
    range_alloc_dump(&alloc);

    assert_equal(alloc.length, 1);
    assert_truth(range_eq(alloc.data[0], final_range));

    range_alloc_deinit(&alloc);
}
