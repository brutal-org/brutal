#pragma once

#include <brutal-text>

// "handover" in ascii
#define HANDOVER_TAG 0x68616e646f766572

typedef enum
{
    HANDOVER_MMAP_FREE,
    HANDOVER_MMAP_USED,
    HANDOVER_MMAP_RECLAIMABLE,
    HANDOVER_MMAP_RESERVED,
} HandoverMmapType;

static inline Str ho_mmtype_to_str(HandoverMmapType type)
{
    switch (type)
    {
    case HANDOVER_MMAP_FREE:
        return str$("FREE");

    case HANDOVER_MMAP_USED:
        return str$("USED");

    case HANDOVER_MMAP_RESERVED:
        return str$("RESERVED");

    case HANDOVER_MMAP_RECLAIMABLE:
        return str$("RECLAIMABLE");

    default:
        return str$("UNKNOWN");
    }
}

typedef struct
{
    union
    {
        Range(size_t);
        USizeRange range;
    };
    HandoverMmapType type;
} HandoverMmapEntry;

typedef struct
{
    size_t size;

#define HANDOVER_MMAP_MAX_SIZE (64)
    HandoverMmapEntry entries[HANDOVER_MMAP_MAX_SIZE];
} HandoverMmap;

typedef struct
{
    bool present;
    uintptr_t addr;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
} HandoverFramebuffer;

typedef struct
{
    size_t size;
    uintptr_t addr;
    StrFix32 module_name;
} HandoverModule;

typedef struct
{
    size_t size;
#define MAX_MODULE_COUNT 16
    HandoverModule module[MAX_MODULE_COUNT];
} HandoverModules;

typedef struct
{
    uint64_t tag; // must be HANDOVER_TAG

    HandoverMmap mmap;
    HandoverFramebuffer framebuffer;
    HandoverModules modules;
    uintptr_t rsdp;
    uintptr_t fdt;
} Handover;

HandoverModule const *handover_find_module(Handover const *handover, Str name);

void handover_dump(Handover const *handover);

void handover_mmap_append(HandoverMmap *self, HandoverMmapEntry entry);

uintptr_t handover_mmap_base(HandoverMmap const *memory_map);

uintptr_t handover_mmap_end(HandoverMmap const *memory_map);
