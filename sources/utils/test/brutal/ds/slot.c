#include <brutal/alloc.h>
#include <brutal/ds.h>
#include "test/test.h"

TEST(slot_acquire_release)
{
    Slot(int) slot;
    slot_init(&slot, test_alloc());

    slot_alloc(&slot);

    slot_deinit(&slot);
}
