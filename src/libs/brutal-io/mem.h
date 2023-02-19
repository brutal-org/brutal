#pragma once

#include <brutal-alloc>

#include "traits.h"

typedef struct
{
    uint8_t *data;
    size_t used;
    size_t capacity;
} IoMem;

void io_mem_init(IoMem *self, size_t capacity, const void *data);

IoReader io_mem_reader(IoMem *self);

IoWriter io_mem_writer(IoMem *self);
