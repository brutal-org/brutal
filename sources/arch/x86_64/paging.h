#pragma once

#include <library/base/macros.h>
#include <library/base/std.h>
#include <library/mem.h>

struct PACKED pml
{
    union
    {
        uint64_t _raw;

        struct PACKED
        {
            bool present : 1;
            bool read_write : 1;
            bool user : 1;
            bool caching : 1;
            bool caching_disable : 1;
            bool accessed : 1;
            bool dirty : 1;
            bool huge_page : 1;
            bool global_page : 1;
            uint8_t _available : 3;
            uint64_t physical : 52;
        };
    };
};

// by default create a present page
static inline struct pml page(uintptr_t physical, size_t flags)
{
    return (struct pml){
        .physical = physical >> 12,
        .user = (flags & BR_MEM_USER) != 0,
        .read_write = (flags & BR_MEM_WRITABLE) != 0,
        .present = true,
        .caching = false,
        .caching_disable = false,
        .accessed = false,
        .dirty = false,
        .huge_page = false,
        .global_page = false,
        ._available = 0};
}

static_assert(sizeof(struct pml) == sizeof(uint64_t), "pml must be 64 bit");

#define PML4_GET_INDEX(addr) (((uint64_t)addr & ((uint64_t)0x1ff << 39)) >> 39)
#define PMLN_GET_INDEX(addr) (((uint64_t)addr & ((uint64_t)0x1ff << 30)) >> 30)
#define PAGE_DIR_GET_INDEX(addr) (((uint64_t)addr & ((uint64_t)0x1ff << 21)) >> 21)
#define PAGE_TABLE_GET_INDEX(addr) (((uint64_t)addr & ((uint64_t)0x1ff << 12)) >> 12)
