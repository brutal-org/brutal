#include <brutal/alloc.h>
#include <brutal/ds.h>
#include "tests/test.h"

TEST(slot_acquire_release)
{
    Slot(int) slot;
    slot_init(&slot, alloc_global());

    slot_alloc(&slot);

    slot_deinit(&slot);
}
