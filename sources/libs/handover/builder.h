#pragma once

#include <bal/boot/handover.h>
#include <brutal/ds.h>
#include <brutal/text.h>

typedef struct
{
    Vec(HandoverRecord) entries;
    Buf strings;
} Ho2Builder;

void handover_builder_init(Ho2Builder *self, Alloc *alloc);

void handover_builder_deinit(Ho2Builder *self);

size_t handover_builder_size(Ho2Builder *self);

size_t handover_builder_finalize(Ho2Builder *self, uint8_t *buf);

void handover_builder_record(Ho2Builder *self, HandoverRecord entry);

void handover_builder_count(Ho2Builder *self);

void handover_builder_reserve(Ho2Builder *self, size_t size);

size_t handover_builder_string(Ho2Builder *self, Str str);
