#pragma once

#include <brutal-alloc>

#include "traits.h"

typedef struct
{
    uint8_t *data;
    size_t pos;
    size_t used;
    size_t capacity;
    Alloc *alloc;
} Buf;

void buf_init(Buf *self, size_t capacity, Alloc *alloc);

void buf_deinit(Buf *self);

void buf_ensure(Buf *self, size_t capacity);

void buf_clear(Buf *self);

void buf_push_impl(Buf *self, uint8_t const *data, size_t size);

uint8_t buf_load_u8(Buf *self, size_t off);

void buf_store_u8(Buf *self, size_t off, uint8_t val);

Str buf_str(Buf *self);

#define buf_putc(SELF, CHR) \
    buf_push(SELF, (char)(CHR))

#define buf_push(SELF, DATA) (                                             \
    {                                                                      \
        typeof(DATA) __data = (DATA);                                      \
        buf_push_impl((SELF), (uint8_t const *)&(__data), sizeof(__data)); \
    })

#define buf_write(SELF, DATA, SIZE) \
    buf_push_impl((SELF), (uint8_t const *)(DATA), (SIZE))

#define buf_begin(SELF) ((SELF)->data)

#define buf_end(SELF) ((SELF)->data + (SELF)->used)

#define buf_used(SELF) ((SELF)->used)

IoReader buf_reader(Buf *self);

IoWriter buf_writer(Buf *self);

IoDuplex buf_duplex(Buf *self);

IoSeeker buf_seeker(Buf *self);

IoRSeek buf_rseek(Buf *self);

// Can't have this yet
// IoWSeek buf_wseek(Buf *self);
