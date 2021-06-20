#pragma once

#include <brutal/text.h>

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

struct handover_framebuffer
{
    bool has_framebuffer;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t bpp;
    uint64_t framebuffer_physical_addr;
};

#define MAX_MODULE_COUNT 64
#define MAX_MODULE_NAME_LENGTH 128

struct handover_module
{
    size_t size;
    uint64_t addr;
    str_fix128_t module_name;
};

struct handover_modules_list
{
    size_t module_count;
    struct handover_module module[MAX_MODULE_COUNT];
};

#define HANDOVER_IDENTIFIER (0x42525554414C00) // equlivalent to brutal0 in ASCII

struct handover
{
    int64_t identifier; // if one day we have a custom bootloader

    struct handover_mmap mmap;
    struct handover_framebuffer framebuffer;
    struct handover_modules_list modules;
    uintptr_t rsdp;
};
