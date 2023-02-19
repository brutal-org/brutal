#pragma once

#include <brutal-io>

typedef struct PACKED
{
    be_uint32_t magic;
    be_uint32_t size;
    be_uint32_t structure_offset;
    be_uint32_t strings_offset;
    be_uint32_t memory_reservation_offset;
    be_uint32_t version;
    be_uint32_t last_comp_version;
    be_uint32_t boot_cpuid_phys;
    be_uint32_t strings_size;
    be_uint32_t structure_size;
} FdtHeader;

typedef struct PACKED
{
    be_uint64_t address;
    be_uint64_t size;
} FdtReservationEntry;

enum
{
    FDT_BEGIN_NODE = 1,
    FDT_END_NODE = 2,
    FDT_PROP = 3,
    FDT_NOP = 4,
    FDT_END = 9
};

typedef be_uint32_t FdtTok;

typedef struct
{
    FdtTok token;
    char name[];
} FdtTokBegin;

typedef struct
{
    FdtTok token;
    be_uint32_t len;
    be_uint32_t name_offset;
    char value[];
} FdtTokProp;

typedef struct
{
    Str name;

    FdtHeader *fdt;
    FdtTok *begin;
    FdtTok *end;
} FdtNode;

typedef struct
{
    Str name;
    VoidSlice value;
} FdtProp;

#define FDT_MAGIC 0xd00dfeed

FdtHeader *fdt_from_data(void *raw_data);

FdtTok *fdt_tok_begin(FdtHeader *fdt);
FdtTok *fdt_tok_next(FdtTok *tok);
FdtTok *fdt_tok_end(FdtHeader *fdt);

FdtNode fdt_node_root(FdtHeader *fdt);

Iter fdt_node_childs(FdtNode node, IterFn fn, void *ctx);
Iter fdt_node_props(FdtNode node, IterFn fn, void *ctx);

FdtNode fdt_lookup_node(FdtHeader *fdt, Str path);
FdtProp fdt_lookup_props(FdtNode node, Str name);

void fdt_dump(FdtHeader *fdt, Emit *out, bool dump_values);

// FIXME: addr_size and len_size are not really supported for the moment
// they should be used to determine the size of the address and length fields in 32bit count (so len_size == 2
// then the size of the len register should be 64bits)
USizeRange fdt_reg_get_range(FdtProp prop, uintptr_t addr_size, uintptr_t len_size);
