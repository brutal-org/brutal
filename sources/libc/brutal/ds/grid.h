#pragma once

#include <brutal/alloc/base.h>

typedef struct
{
    int width;
    int height;
    void *data;
    int data_size;
    Alloc *alloc;
} GridImpl;

#define Grid(T)         \
    union               \
    {                   \
        struct          \
        {               \
            int width;  \
            int height; \
            T *data;    \
        };              \
        GridImpl _impl; \
    }

void grid_init_impl(GridImpl *impl, int width, int height, int data_size, Alloc *alloc);

void grid_deinit_impl(GridImpl *impl);

void grid_resize_impl(GridImpl *impl, int width, int height);

#define grid_init(SELF, WIDTH, HEIGHT, ALLOC) \
    grid_init_impl(impl_cast(SELF), (WIDTH), (HEIGHT), sizeof(*(SELF)->data), (ALLOC))

#define grid_deinit(SELF) \
    grid_deinit_impl(impl_cast(SELF))

#define grid_resize(SELF, WIDTH, HEIGHT) \
    grid_resize_impl(impl_cast(SELF), (WIDTH), (HEIGHT))

#define grid_fill(SELF, DATA)                                           \
    for (int __it = 0; __it < ((SELF)->width * (SELF)->height); __it++) \
    {                                                                   \
        (SELF)->data[__it] = (DATA);                                    \
    }
