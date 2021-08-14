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

#define EFI_ERR 0x8000000000000000
#define EFI_LOAD_ERROR (EFI_ERR | 0x0000000000000001)
#define EFI_INVALID_PARAMETER (EFI_ERR | 0x0000000000000002)
#define EFI_UNSUPPORTED (EFI_ERR | 0x0000000000000003)
#define EFI_BAD_BUFFER_SIZE (EFI_ERR | 0x0000000000000004)
#define EFI_BUFFER_TOO_SMALL (EFI_ERR | 0x0000000000000005)
#define EFI_NOT_READY (EFI_ERR | 0x0000000000000006)
#define EFI_DEVICE_ERROR (EFI_ERR | 0x0000000000000007)
#define EFI_WRITE_PROTECTED (EFI_ERR | 0x0000000000000008)
#define EFI_OUT_OF_RESOURCES (EFI_ERR | 0x0000000000000009)
#define EFI_VOLUME_CORRUPTED (EFI_ERR | 0x000000000000000a)
#define EFI_VOLUME_FULL (EFI_ERR | 0x000000000000000b)
#define EFI_NO_MEDIA (EFI_ERR | 0x000000000000000c)
#define EFI_MEDIA_CHANGED (EFI_ERR | 0x000000000000000d)
#define EFI_NOT_FOUND (EFI_ERR | 0x000000000000000e)
#define EFI_ACCESS_DENIED (EFI_ERR | 0x000000000000000f)
#define EFI_NO_RESPONSE (EFI_ERR | 0x0000000000000010)
#define EFI_NO_MAPPING (EFI_ERR | 0x0000000000000011)
#define EFI_TIMEOUT (EFI_ERR | 0x0000000000000012)
#define EFI_NOT_STARTED (EFI_ERR | 0x0000000000000013)
#define EFI_ALREADY_STARTED (EFI_ERR | 0x0000000000000014)
#define EFI_ABORTED (EFI_ERR | 0x0000000000000015)
#define EFI_ICMP_ERROR (EFI_ERR | 0x0000000000000016)
#define EFI_TFTP_ERROR (EFI_ERR | 0x0000000000000017)
#define EFI_PROTOCOL_ERROR (EFI_ERR | 0x0000000000000018)
#define EFI_INCOMPATIBLE_VERSION (EFI_ERR | 0x0000000000000019)
#define EFI_SECURITY_VIOLATION (EFI_ERR | 0x000000000000001a)
#define EFI_CRC_ERROR (EFI_ERR | 0x000000000000001b)
#define EFI_END_OF_MEDIA (EFI_ERR | 0x000000000000001c)
#define EFI_END_OF_FILE (EFI_ERR | 0x000000000000001f)
#define EFI_INVALID_LANGUAGE (EFI_ERR | 0x0000000000000020)
#define EFI_COMPROMISED_DATA (EFI_ERR | 0x0000000000000021)
#define EFI_IP_ADDRESS_CONFLICT (EFI_ERR | 0x0000000000000022)
#define EFI_HTTP_ERROR (EFI_ERR | 0x0000000000000023)
