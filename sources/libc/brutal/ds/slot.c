#include <brutal/alloc/base.h>
#include <brutal/ds/slot.h>

void slot_capacity_impl(struct slot_impl *impl, int new_capacity)
{
    if (impl->capacity >= new_capacity)
    {
        return;
    }

    auto new_data = alloc_calloc(impl->alloc, impl->data_size, new_capacity);
    auto new_used = alloc_make_array(impl->alloc, bool, new_capacity);

    if (impl->data)
    {
        mem_cpy(new_data, impl->data, impl->data_size * new_capacity);
        mem_cpy(new_used, impl->used, sizeof(bool) * new_capacity);

        alloc_release(impl->alloc, impl->data);
        alloc_release(impl->alloc, impl->used);
    }

    impl->data = new_data;
    impl->used = new_used;

    impl->capacity = new_capacity;
}

void slot_init_impl(struct slot_impl *impl, int data_size, struct alloc *alloc)
{
    *impl = (struct slot_impl){};

    impl->alloc = alloc;
    impl->data_size = data_size;
}

void slot_deinit_impl(struct slot_impl *impl)
{
    if (impl->data)
    {
        alloc_release(impl->alloc, impl->data);
    }

    if (impl->used)
    {
        alloc_release(impl->alloc, impl->used);
    }

    *impl = (struct slot_impl){};
}

slot_index_t slot_find_impl(struct slot_impl *impl)
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

slot_index_t slot_acquire_impl(struct slot_impl *impl)
{
    slot_capacity_impl(impl, 16);

    slot_index_t found_index = slot_find_impl(impl);

    if (found_index < 0)
    {
        return found_index;
    }

    slot_capacity_impl(impl, impl->capacity + impl->capacity / 4);

    return slot_find_impl(impl);
}

void slot_release_impl(struct slot_impl *impl, slot_index_t index)
{
    impl->used[index] = false;
}
