#include <handover/builder.h>

void handover_builder_init(Ho2Builder *self, Alloc *alloc)
{
    vec_init(&self->entries, alloc);
    buf_init(&self->strings, 128, alloc);
}

void handover_builder_deinit(Ho2Builder *self)
{
    vec_deinit(&self->entries);
    buf_deinit(&self->strings);
}

size_t handover_builder_size(Ho2Builder *self)
{
    return sizeof(Handover) +
           vec_len(&self->entries) * sizeof(HandoverRecord) +
           buf_used(&self->strings);
}

/* TODO:
size_t handover_builder_finalize(Ho2Builder *self, uint8_t *buf)
{
}
*/

void handover_builder_record(Ho2Builder *self, HandoverRecord record)
{
    vec_push(&self->entries, record);
}

size_t handover_builder_count(Ho2Builder *self)
{
    return vec_len(&self->entries);
}

void handover_builder_reserve(Ho2Builder *self, size_t size)
{
    vec_reserve(&self->entries, vec_len(&self->entries) + size);
}

size_t handover_builder_string(Ho2Builder *self, Str str)
{
    size_t offset = buf_used(&self->strings);
    buf_write(&self->strings, str.buf, str.len);
    buf_push(&self->strings, '\0');
    return offset;
}
