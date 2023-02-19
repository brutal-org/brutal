#pragma once

#include <bal/abi.h>
#include <brutal-mem>

typedef union PACKED
{
    struct PACKED
    {
        bool present : 1;
        bool read_write : 1;
        bool user : 1;
        bool page_write_through : 1;
        bool caching_disable : 1;
        bool accessed : 1;
        bool dirty : 1;
        bool huge_page : 1;
        bool global_page : 1;
        uint8_t _available : 3;
        uint64_t physical : 52;
    };

    uint64_t _raw;
} PmlEntry;

typedef struct PACKED
{
    PmlEntry entries[512];
} Pml;

// by default create a present page
static inline PmlEntry pml_make_entry(uintptr_t physical, size_t flags)
{
    return (PmlEntry){
        .physical = physical >> 12,
        .user = (flags & BR_MEM_USER) != 0,
        .read_write = (flags & BR_MEM_WRITABLE) != 0,
        .present = true,
        .page_write_through = (flags & BR_MEM_WRITE_THROUGHT) != 0,
        .caching_disable = false,
        .accessed = false,
        .dirty = false,
        .huge_page = false,
        .global_page = (flags & BR_MEM_GLOBAL) != 0,
        ._available = 0,
    };
}

static inline PmlEntry pml_clean_entry(void)
{
    return (PmlEntry){};
}

static_assert(sizeof(PmlEntry) == sizeof(uint64_t), "pml must be 64 bit");

#define PMLX_GET_INDEX(ADDR, LEVEL) (((uint64_t)ADDR & ((uint64_t)0x1ff << (12 + LEVEL * 9))) >> (12 + LEVEL * 9))

#define PML4_GET_INDEX(ADDR) PMLX_GET_INDEX(ADDR, 3)
#define PML3_GET_INDEX(ADDR) PMLX_GET_INDEX(ADDR, 2)
#define PML2_GET_INDEX(ADDR) PMLX_GET_INDEX(ADDR, 1)
#define PML1_GET_INDEX(ADDR) PMLX_GET_INDEX(ADDR, 0)

#define PML_GET_ADDR(PML) ((PML).physical << 12)
