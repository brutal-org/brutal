#pragma once

#include <virtio/splitvqueue.h>
#include <brutal-base>

// The descriptor of one buffer inside the queue.
typedef struct PACKED
{
    le_uint64_t address;
    le_uint32_t len;

    le_uint16_t flags;
    le_uint16_t next;
} VirtioVQueueDesc;
