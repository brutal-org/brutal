#pragma once

#include <brutal/alloc/base.h>
#include <brutal/host/io.h>
#include <brutal/io/read.h>
#include <brutal/io/write.h>

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

Str buffer_str(Buffer *self);

#define buffer_putc(SELF, CHR) \
    buffer_push(SELF, (char)(CHR))

#define buffer_push(SELF, DATA) (                                             \
    {                                                                         \
        typeof(DATA) __data = (DATA);                                         \
        buffer_push_impl((SELF), (uint8_t const *)&(__data), sizeof(__data)); \
    })

#define buffer_write(SELF, DATA, SIZE) \
    buffer_push_impl((SELF), (uint8_t const *)(DATA), (SIZE))

#define buffer_begin(SELF) ((SELF)->data)

#define buffer_end(SELF) ((SELF)->data + (SELF)->used)

#define buffer_used(SELF) ((SELF)->used)

typedef struct
{
    IoReader base;
    Buffer const *buf;
    size_t cur;
} IoBufferReader;

typedef struct
{
    IoWriter base;
    Buffer *buf;
} IoBufferWriter;

IoBufferReader io_buffer_read(Buffer const *self);

IoBufferWriter io_buffer_write(Buffer *self);
