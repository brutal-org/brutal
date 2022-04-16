#pragma once

#include <brutal-alloc>

typedef struct
{
    uint8_t *data;
    int head;
    int tail;
    int used;
    int capacity;
    int data_size;
    Alloc *alloc;
} RingImpl;

#define Ring(T)           \
    union                 \
    {                     \
        struct            \
        {                 \
            T *data;      \
            int head;     \
            int tail;     \
            int used;     \
            int capacity; \
        };                \
        RingImpl _impl;   \
    }

void ring_init_impl(RingImpl *impl, int capacity, int data_size, Alloc *alloc);

void ring_deinit_impl(RingImpl *impl);

bool ring_push_impl(RingImpl *impl, uint8_t const *data);

bool ring_pop_impl(RingImpl *impl, uint8_t *data);

void ring_clear_impl(RingImpl *impl);

int ring_len_impl(RingImpl *impl);

#define ring_init(SELF, CAPACITY, ALLOC) \
    ring_init_impl(impl$(SELF), CAPACITY, sizeof((SELF)->data[0]), ALLOC)

#define ring_deinit(SELF) \
    ring_deinit_impl(impl$(SELF))

#define ring_push(SELF, DATA) \
    ring_push_impl(impl$(SELF), (uint8_t const *)(DATA))

#define ring_pop(SELF, DATA) \
    ring_pop_impl(impl$(SELF), (uint8_t *)(DATA))

#define ring_clear(SELF) \
    ring_clear_impl(impl$(SELF))

#define ring_len(SELF) \
    (SELF)->used

#define ring_full(SELF) \
    ((SELF)->used == (SELF)->capacity)

#define ring_any(SELF) \
    ((SELF)->used != 0)
