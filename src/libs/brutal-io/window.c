#include <brutal-debug>
#include <brutal-math>

#include "funcs.h"
#include "window.h"

#define WINDOW_MAX_CAPACITY_FACTOR 3
#define WINDOW_FLUSH_THRESHOLD_FACTOR 2

void io_window_init(IoWindow *self, IoWriter underlying, size_t capacity, Alloc *alloc)
{
    *self = (IoWindow){
        .data = (uint8_t *)alloc_malloc(alloc, capacity * WINDOW_MAX_CAPACITY_FACTOR),
        .underlying = underlying,
        .used = 0,
        .capacity = capacity,
        .alloc = alloc,
    };
}

void io_window_deinit(IoWindow *self)
{
    alloc_free(self->alloc, self->data);
}

void io_window_flush_chunk(IoWindow *self)
{
    size_t src_size = m_min(self->used, self->capacity);
    size_t src_start = src_size > self->capacity ? src_size - self->capacity : 0;
    uint8_t *src = self->data + src_start;
    io_write(self->underlying, src, src_size);
    self->used -= src_size;
    mem_move(self->data, src + src_size, self->used);
}

void io_window_flush_all(IoWindow *self)
{
    io_write(self->underlying, self->data, self->used);
    self->used = 0;
}

static IoResult io_window_write_impl(IoWindow *self, char const *data, size_t size)
{
    size_t to_write = m_min((self->capacity * WINDOW_MAX_CAPACITY_FACTOR) - self->used, size);
    mem_cpy(self->data + self->used, data, to_write);
    self->used += to_write;

    // Put the latest data back to the front
    if (self->used > self->capacity * WINDOW_FLUSH_THRESHOLD_FACTOR)
    {
        io_window_flush_chunk(self);
    }
    return OK(IoResult, to_write);
}

IoWriter io_window_writer(IoWindow *self)
{
    return (IoWriter){
        .write = (IoWriteFn *)io_window_write_impl,
        .context = self,
    };
}

uint8_t io_window_peek_from_back(IoWindow *self, size_t offset)
{
    assert_greater_equal(self->used, offset);
    return self->data[self->used - offset];
}
