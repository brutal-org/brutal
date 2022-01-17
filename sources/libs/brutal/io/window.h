#pragma once

#include <brutal/alloc/base.h>
#include <brutal/io/write.h>

typedef struct
{
    uint8_t *data;
    size_t used;
    size_t capacity;
    IoWriter underlying;
    Alloc *alloc;
} Window;

void window_init(Window *self, IoWriter underlying, size_t windowsize, Alloc *alloc);
void window_deinit(Window *self);
void window_flush(Window *self);

uint8_t window_peek_from_back(Window *self, size_t offset);
IoWriter window_writer(Window *self);
