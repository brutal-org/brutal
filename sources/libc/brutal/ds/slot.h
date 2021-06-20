#pragma once

#include <brutal/base.h>

typedef int slot_index_t;

struct slot_impl
{
    void *data;
    bool *used;

    struct alloc *alloc;
    int data_size;
    int capacity;
};

#define slot_t(T)               \
    union                       \
    {                           \
        T *data;                \
        struct slot_impl _impl; \
    }

void slot_init_impl(struct slot_impl *impl, int data_size, struct alloc *alloc);

void slot_deinit_impl(struct slot_impl *impl);

slot_index_t slot_alloc_impl(struct slot_impl *impl);

void slot_acquire_impl(struct slot_impl *impl, size_t index);

void slot_release_impl(struct slot_impl *impl, slot_index_t index);

#define slot_init(self, alloc) \
    slot_init_impl(&(self)->_impl, sizeof(&(self)->data), (alloc))

#define slot_deinit(self) \
    slot_deinit_impl(&(self)->_impl)

#define slot_alloc(self) \
    slot_alloc_impl(&(self)->impl)

#define slot_acquire(self, index) \
    slot_acquire_impl(&(self)->_impl, index)

#define slot_release(self, index) \
    slot_release_impl(&(self)->_impl, index)

#define slot_at(self, index) \
    ({ &self->data[index]; })
