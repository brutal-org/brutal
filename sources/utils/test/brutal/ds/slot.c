#include <bruwutal/alloc.h>
#include <bruwutal/ds.h>
#include "test/test.h"

TEST(slot_acquire_release)
{
    Slot(int) slot;
    slot_init(&slot, test_alloc());

    slot_alloc(&slot);

    slot_deinit(&slot);
}
