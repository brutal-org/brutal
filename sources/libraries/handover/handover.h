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
    StrFix128 module_name;
} HandoverModule;

typedef struct
{
    size_t size;
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

void handover_dump(Handover *handover);
