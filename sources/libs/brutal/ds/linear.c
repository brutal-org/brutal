#include <brutal/ds/linear.h>

void linear_buf_init_impl(LinearBufImpl *impl, Alloc *alloc)
{
    *impl = (LinearBufImpl){};
    buf_init(&impl->buf, 512, alloc);
}

void linear_buf_deinit_impl(LinearBufImpl *impl)
{
    buf_deinit(&impl->buf);
    *impl = (LinearBufImpl){};
}

void linear_buf_push_impl(LinearBufImpl *impl, uint8_t const *data, size_t size)
{
    impl->current = buf_used(&impl->buf);
    buf_push(&impl->buf, (LinearBufNode){.size = size + sizeof(LinearBufNode)});
    buf_push_impl(&impl->buf, data, size);
}

void linear_buf_attach_impl(LinearBufImpl *impl, uint8_t const *data, size_t size)
{
    LinearBufNode *current = (LinearBufNode *)buf_begin(&impl->buf) + impl->current;
    current->size += size;
    buf_write(&impl->buf, data, size);
}

void linear_buf_clear_impl(LinearBufImpl *impl)
{
    buf_clear(&impl->buf);
    impl->current = 0;
}
