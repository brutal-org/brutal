#pragma once

#include <brutal/text.h>

typedef enum
{
    HANDOVER_MMAP_FREE,
    HANDOVER_MMAP_KERNEL_MODULE,
    HANDOVER_MMAP_RESERVED,
    HANDOVER_MMAP_RECLAIMABLE,
    HANDOVER_MMAP_FRAMEBUFFER,
} HandoverMmapType;

typedef struct
{
    HandoverMmapType type;

    uintptr_t base;
    size_t length;
} HandoverMmapEntry;

typedef struct
{
    size_t size;

#define HANDOVER_MMAP_MAX_SIZE (64)
    HandoverMmapEntry entries[HANDOVER_MMAP_MAX_SIZE];
} HandoverMmap;

typedef struct
{
    bool has_framebuffer;
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uintptr_t addr;
} HandoverFramebuffer;

typedef struct
{
    size_t size;
    uintptr_t addr;
    StrFix128 module_name;
} HandoverModule;

typedef struct
{
    size_t module_count;
#define MAX_MODULE_COUNT 16
    HandoverModule module[MAX_MODULE_COUNT];
} HandoverModules;

typedef struct
{
    HandoverMmap mmap;
    HandoverFramebuffer framebuffer;
    HandoverModules modules;
    uintptr_t rsdp;
} Handover;

HandoverModule *handover_find_module(Handover *handover, Str name);
