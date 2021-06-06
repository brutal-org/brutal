#pragma once

#include <stddef.h>
#include <stdint.h>

// I don't think we will get over 64
#define HANDOVER_MMAP_MAX_SIZE 64

enum handover_mmap_entry_type
{
    HANDOVER_MMAP_FREE,
    HANDOVER_MMAP_KERNEL_MODULE,
    HANDOVER_MMAP_RESERVED,
    HANDOVER_MMAP_RECLAIMABLE,
    HANDOVER_MMAP_FRAMEBUFFER,
};

struct handover_mmap_entry
{
    enum handover_mmap_entry_type type;
    uintptr_t base;
    uintptr_t length;
};

struct handover_mmap
{
    size_t size;
    struct handover_mmap_entry entries[HANDOVER_MMAP_MAX_SIZE];
};

struct handover
{
    struct handover_mmap mmap;
    uintptr_t rsdp;
};
