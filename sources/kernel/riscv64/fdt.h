#pragma once
#include <brutal/base.h>
#include <brutal/base/endian.h>
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

typedef enum
{
    FDT_BEGIN_NODE = 1,
    FDT_END_NODE = 2,
    FDT_PROP = 3,
    FDT_NOP = 4,
    FDT_END = 9
} FdtNodeType;

typedef struct PACKED
{
    be_uint32_t type;
    char name[];
} FdtBeginNode;

typedef struct PACKED
{
    be_uint32_t type;
    be_uint32_t len;
    be_uint32_t name_offset;
    char value[];
} FdtPropertiesNode;

typedef struct
{
    Str property_name;
    size_t property_value_size;
    void *property_value;
} NodeProperty;

#define FDT_MAGIC 0xd00dfeed

FdtBeginNode *fdt_root(FdtHeader *fdt);

void dump_fdt(FdtHeader *header, FdtBeginNode *current_fdt);

FdtBeginNode *get_fdt_node(FdtBeginNode *node, Str entry);

NodeProperty get_fdt_property(FdtHeader *header, FdtBeginNode *node, Str name);
