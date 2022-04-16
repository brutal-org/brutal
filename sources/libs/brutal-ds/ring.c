#include "ring.h"

void ring_init_impl(RingImpl *impl, int capacity, int data_size, Alloc *alloc)
{
    *impl = (RingImpl){
        .data = (uint8_t *)alloc_calloc(alloc, data_size, capacity),
        .head = 0,
        .tail = 0,
        .used = 0,
        .capacity = capacity,
        .data_size = data_size,
        .alloc = alloc,
    };
}

void ring_deinit_impl(RingImpl *impl)
{
    alloc_free(impl->alloc, impl->data);
    *impl = (RingImpl){};
}

bool ring_push_impl(RingImpl *impl, uint8_t const *data)
{
    if (impl->used == impl->capacity)
    {
        return false;
    }

    mem_cpy(impl->data + impl->head * impl->data_size, data, impl->data_size);
    impl->head = (impl->head + 1) % impl->capacity;
    impl->used++;

    return true;
}

bool ring_pop_impl(RingImpl *impl, uint8_t *data)
{
    if (impl->used == 0)
    {
        return false;
    }

    mem_cpy(data, impl->data + impl->tail * impl->data_size, impl->data_size);
    impl->tail = (impl->tail + 1) % impl->capacity;
    impl->used--;

    return true;
}

void ring_clear_impl(RingImpl *impl)
{
    impl->head = 0;
    impl->tail = 0;
    impl->used = 0;
}

int ring_len_impl(RingImpl *impl)
{
    return impl->used;
}
