#pragma once

#include <brutal/alloc/base.h>
#include <brutal/io/read.h>
#include <brutal/io/write.h>

typedef struct
{
    uint8_t *data;
    size_t used;
    size_t capacity;
} MemView;

void mem_view_init(MemView *self, size_t capacity, const void *data);

IoReader mem_view_reader(MemView *self);

IoWriter mem_view_writer(MemView *self);
