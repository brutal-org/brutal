#include <handover/builder.h>
#include <brutal-debug>

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

void handover_builder_finalize(Ho2Builder *self, uint8_t *buf)
{
    Handover *handover = (Handover *)buf;

    handover->magic = HANDOVER_MAGIC;
    handover->agent = self->agent;
    handover->size = handover_builder_size(self);
    handover->count = vec_len(&self->entries);

    buf += sizeof(Handover);

    for (int i = 0; i < vec_len(&self->entries); i++)
    {
        *((HandoverRecord *)buf) = self->entries.data[i];
        buf += sizeof(HandoverRecord);
    }

    mem_cpy(buf, self->strings.data, buf_used(&self->strings));
}

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

void handover_builder_agent(Ho2Builder *self, Str agent)
{
    assert_equal(self->agent, 0);
    self->agent = handover_builder_string(self, agent);
}
