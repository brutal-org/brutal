#pragma once

#include <brutal-alloc>

#include "traits.h"

/**
    @brief A sliding window implementation, used to lookup already written data

    This struct allocates memory depending on the capacity specified.
    Sample applications are compression algorithms (e.g. DEFLATE)
*/
typedef struct
{
    uint8_t *data;
    size_t used;
    size_t capacity;
    IoWriter underlying;
    Alloc *alloc;
} IoWindow;

void io_window_init(IoWindow *self, IoWriter underlying, size_t windowsize, Alloc *alloc);

void io_window_deinit(IoWindow *self);

void io_window_flush_chunk(IoWindow *self);

void io_window_flush_all(IoWindow *self);

uint8_t io_window_peek_from_back(IoWindow *self, size_t offset);

IoWriter io_window_writer(IoWindow *self);
