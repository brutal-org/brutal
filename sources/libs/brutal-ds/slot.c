#include <brutal-debug>

#include "slot.h"

void slot_capacity_impl(SlotImpl *impl, int new_capacity)
{
    if (impl->capacity >= new_capacity)
    {
        return;
    }

    void *new_data = alloc_calloc(impl->alloc, impl->data_size, new_capacity);
    bool *new_used = alloc_make_array(impl->alloc, bool, new_capacity);

    if (impl->data)
    {
        mem_cpy(new_data, impl->data, impl->data_size * impl->capacity);
        mem_cpy(new_used, impl->used, sizeof(bool) * impl->capacity);

        alloc_release(impl->alloc, impl->data);
        alloc_release(impl->alloc, impl->used);
    }

    impl->data = new_data;
    impl->used = new_used;

    impl->capacity = new_capacity;
}

void slot_init_impl(SlotImpl *impl, int data_size, Alloc *alloc)
{
    *impl = (SlotImpl){};

    impl->alloc = alloc;
    impl->data_size = data_size;
}

void slot_deinit_impl(SlotImpl *impl)
{
    if (impl->data)
    {
        alloc_release(impl->alloc, impl->data);
    }

    if (impl->used)
    {
        alloc_release(impl->alloc, impl->used);
    }

    *impl = (SlotImpl){};
}

SlotIndex slot_find_impl(SlotImpl *impl)
{
    for (int i = 0; i < impl->capacity; i++)
    {
        if (!impl->used[i])
        {
            impl->used[i] = true;
            return i;
        }
    }

    return -1;
}

SlotIndex slot_alloc_impl(SlotImpl *impl)
{
    slot_capacity_impl(impl, 16);

    SlotIndex found_index = slot_find_impl(impl);

    if (found_index >= 0)
    {
        return found_index;
    }

    slot_capacity_impl(impl, impl->capacity + impl->capacity / 4);

    return slot_find_impl(impl);
}

void slot_acquire_impl(SlotImpl *impl, size_t index)
{
    assert_falsity(impl->used[index]);
    impl->used[index] = true;
}

void slot_release_impl(SlotImpl *impl, SlotIndex index)
{
    assert_truth(impl->used[index]);
    impl->used[index] = false;
}

bool slot_valid_impl(SlotImpl *impl, SlotIndex index)
{
    return index >= 0 && index < impl->capacity && impl->used[index];
}
