#pragma once
#include <bal/abi.h>

/* here we assume paging at level Sv39 and beyond, see spec Supervisor-level ISA 4.4 */

typedef union PACKED
{
    struct PACKED
    {
        bool valid : 1;
        bool read : 1;
        bool write : 1;
        bool executable : 1;
        bool user : 1;
        bool global : 1;
        bool access : 1;
        bool dirty : 1;
        uint8_t rsw : 2;
        uint64_t ppn : 44;
        uint64_t _reserved : 7;
        uint8_t pbmt : 2;
        uint8_t n : 1;
    };
    uint64_t _raw;
} SvPage;

static_assert(sizeof(SvPage) == sizeof(uint64_t), "svPage must be 64bit !!!!");

typedef struct PACKED
{
    SvPage pages[512];
} SvPageTable;

SvPage sv_make_entry(BrMemoryFlags flags, uintptr_t phys, bool is_leaf)
{
    return (SvPage){
        .valid = 1,
        .read = is_leaf && (bool)(flags & BR_MEM_READABLE),
        .write = is_leaf && (bool)(flags & BR_MEM_WRITABLE),
        .executable = is_leaf && (bool)(flags & BR_MEM_EXECUTABLE),
        .user = (bool)(flags & BR_MEM_USER),
        .ppn = (uintptr_t)(phys >> 12),
        .global = 0,
    };
}

#define PAGING_SV32 2
#define PAGING_SV39 3
#define PAGING_SV48 4
#define PAGING_SV57 5

#define DEFAULT_PAGING_DEPTH (PAGING_SV39)

#define SVTABLE_GET_INDEX(ADDR, LEVEL) (((uint64_t)ADDR & ((uint64_t)0x1ff << (12 + LEVEL * 9))) >> (12 + LEVEL * 9))
