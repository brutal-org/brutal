#include <brutal/ds/linear.h>

void linear_buffer_init_impl(LinearBufferImpl *impl, Alloc *alloc)
{
    *impl = (LinearBufferImpl){};
    buffer_init(&impl->buffer, 512, alloc);
}

void linear_buffer_deinit_impl(LinearBufferImpl *impl)
{
    buffer_deinit(&impl->buffer);
    *impl = (LinearBufferImpl){};
}

void linear_buffer_push_impl(LinearBufferImpl *impl, uint8_t const *data, size_t size)
{
    impl->current = buffer_used(&impl->buffer);
    buffer_push(&impl->buffer, (LinearBufferNode){.size = size + sizeof(LinearBufferNode)});
    buffer_push_impl(&impl->buffer, data, size);
}

void linear_buffer_attach_impl(LinearBufferImpl *impl, uint8_t const *data, size_t size)
{
    LinearBufferNode *current = (LinearBufferNode *)buffer_begin(&impl->buffer) + impl->current;
    current->size += size;
    buffer_write(&impl->buffer, data, size);
}

void linear_buffer_clear_impl(LinearBufferImpl *impl)
{
    buffer_clear(&impl->buffer);
    impl->current = 0;
}
