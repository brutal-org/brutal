#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define EFI_SUCCESS 0

#define UNUSED(x) (void)x

typedef unsigned short char16;
typedef unsigned long long EFIStatus;
typedef void *EFIEvent;
typedef void *EFIHandle;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct
{
    u16 year;
    u8 month;
    u8 pad1;
    u32 nanosecond;
    int16_t time_zone;
    u8 daylight;
    u8 pad2;
} EFITime;

typedef struct
{
    u32 resolution;
    u32 accuracy;
    bool sets_to_zero;
} EFITimeCapabilities;

typedef struct
{
    u32 data1;
    u16 data2;
    u16 data3;
    u8 data4[8];
} EFIGUID;

typedef struct
{
    u32 type;
    u64 physical_start;
    u64 virtual_start;
    u64 num_pages;
    u64 attribute;
} EFIMemoryDescriptor;

typedef struct
{
    u64 signature;
    u32 revision;
    u32 header_size;
    u32 CRC32;
    u32 reserved;
} EFITableHeader;
