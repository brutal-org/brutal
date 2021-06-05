#pragma once
#include <stddef.h>
#include <stdint.h>

// i don't think we will get over 64
#define HANDOVER_MMAP_MAX_SIZE 64


enum handover_mmap_entry_type 
{
    HANDOVER_MMAP_FREE = 0,
    HANDOVER_MMAP_KERNEL_MODULE = 1, // kernel and module
    HANDOVER_MMAP_RESERVED = 2,
    HANDOVER_MMAP_RECLAIMABLE = 3,
    HANDOVER_MMAP_FRAMEBUFFER = 4,
};


struct handover_mmap_entry
{
    int type;
    uintptr_t base;
    uintptr_t length;
};

struct handover_mmap
{
    size_t size;
    struct handover_mmap_entry mmap_table[HANDOVER_MMAP_MAX_SIZE];
};
struct handover
{
    void *raw;
    struct handover_mmap mmap;
};