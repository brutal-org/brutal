#pragma once

#include <brutal/alloc/base.h>

typedef struct
{
    uint8_t *data;
    size_t used;
    size_t capacity;
    Alloc *alloc;
} Buffer;

void buffer_init(Buffer *self, size_t capacity, Alloc *alloc);

void buffer_deinit(Buffer *self);

void buffer_ensure(Buffer *self, size_t capacity);

void buffer_clear(Buffer *self);

void buffer_push_impl(Buffer *self, uint8_t const *data, size_t size);

#define buffer_push(SELF, DATA) buffer_push_impl((SELF), (uint8_t const *)&(DATA), sizeof(DATA))

#define buffer_write(...) buffer_push_impl(__VA_ARGS__)

#define buffer_begin(SELF) ((SELF)->data)

#define buffer_end(SELF) ((SELF)->data + (SELF)->used)

#define buffer_used(SELF) ((SELF)->used)
