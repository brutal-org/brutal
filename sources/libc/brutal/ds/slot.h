#pragma once

#include <brutal/base.h>

typedef int SlotIndex;

typedef struct
{
    void *data;
    bool *used;

    Alloc *alloc;
    int data_size;
    int capacity;
} SlotImpl;

#define Slot(T)         \
    union               \
    {                   \
        T *data;        \
        SlotImpl _impl; \
    }

void slot_init_impl(SlotImpl *impl, int data_size, Alloc *alloc);

void slot_deinit_impl(SlotImpl *impl);

SlotIndex slot_alloc_impl(SlotImpl *impl);

void slot_acquire_impl(SlotImpl *impl, size_t index);

void slot_release_impl(SlotImpl *impl, SlotIndex index);

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

#define slot_foreach(v, self)                                        \
    if ((self)->impl.capacity > 0)                                   \
        for (int i = 0, typeof((self)->data[0]) v = (self)->data[i]; \
             i < (self)->impl.capacity;                              \
             i++, v = (self)->data[i])                               \
            if ((self)->impl.used[i])
