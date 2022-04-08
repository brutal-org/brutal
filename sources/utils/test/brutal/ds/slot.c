#include <brutal/ds.h>
#include <brutal/tests.h>

test$(slot_acquire_release)
{
    Slot(int) slot;
    slot_init(&slot, test_use_alloc());

    slot_alloc(&slot);

    slot_deinit(&slot);
}
