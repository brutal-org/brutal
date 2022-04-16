#pragma once

#include <string.h>
#include <brutal-base>

typedef struct
{
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
} EfiGuid;

static inline bool efi_guid_eq(EfiGuid lhs, EfiGuid rhs)
{
    return memcmp(&lhs, &rhs, sizeof(EfiGuid)) == 0;
}

typedef uint64_t EfiStatus;
typedef void *EFIEvent;
typedef void *EFIHandle;

// https://uefi.org/sites/default/files/resources/UEFI%20Spec%202.8B%20May%202020.pdf part 8.3

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t _pad1;
    uint32_t nanosecond;
    int16_t time_zone;
    uint8_t daylight;
    uint8_t _pad2;
} EFITime;

typedef struct
{
    uint32_t resolution;
    uint32_t accuracy;
    bool sets_to_zero;
} EFITimeCapabilities;

typedef struct
{
    uint32_t type;
    uint64_t physical_start;
    uint64_t virtual_start;
    uint64_t num_pages;
    uint64_t attribute;
} EFIMemoryDescriptor;

typedef struct
{
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t CRC32;
    uint32_t _reserved;
} EFITableHeader;

#define EFI_SUCCESS 0
#define EFI_ERR (1ull << 63)
#define EFI_LOAD_ERROR (EFI_ERR | 0x1)
#define EFI_INVALID_PARAMETER (EFI_ERR | 0x2)
#define EFI_UNSUPPORTED (EFI_ERR | 0x3)
#define EFI_BAD_BUFFER_SIZE (EFI_ERR | 0x4)
#define EFI_BUFFER_TOO_SMALL (EFI_ERR | 0x5)
#define EFI_NOT_READY (EFI_ERR | 0x6)
#define EFI_DEVICE_ERROR (EFI_ERR | 0x7)
#define EFI_WRITE_PROTECTED (EFI_ERR | 0x8)
#define EFI_OUT_OF_RESOURCES (EFI_ERR | 0x9)
#define EFI_VOLUME_CORRUPTED (EFI_ERR | 0xa)
#define EFI_VOLUME_FULL (EFI_ERR | 0xb)
#define EFI_NO_MEDIA (EFI_ERR | 0xc)
#define EFI_MEDIA_CHANGED (EFI_ERR | 0xd)
#define EFI_NOT_FOUND (EFI_ERR | 0xe)
#define EFI_ACCESS_DENIED (EFI_ERR | 0xf)
#define EFI_NO_RESPONSE (EFI_ERR | 0x10)
#define EFI_NO_MAPPING (EFI_ERR | 0x11)
#define EFI_TIMEOUT (EFI_ERR | 0x12)
#define EFI_NOT_STARTED (EFI_ERR | 0x13)
#define EFI_ALREADY_STARTED (EFI_ERR | 0x14)
#define EFI_ABORTED (EFI_ERR | 0x15)
#define EFI_ICMP_ERROR (EFI_ERR | 0x16)
#define EFI_TFTP_ERROR (EFI_ERR | 0x17)
#define EFI_PROTOCOL_ERROR (EFI_ERR | 0x18)
#define EFI_INCOMPATIBLE_VERSION (EFI_ERR | 0x19)
#define EFI_SECURITY_VIOLATION (EFI_ERR | 0x1a)
#define EFI_CRC_ERROR (EFI_ERR | 0x1b)
#define EFI_END_OF_MEDIA (EFI_ERR | 0x1c)
#define EFI_END_OF_FILE (EFI_ERR | 0x1f)
#define EFI_INVALID_LANGUAGE (EFI_ERR | 0x20)
#define EFI_COMPROMISED_DATA (EFI_ERR | 0x21)
#define EFI_IP_ADDRESS_CONFLICT (EFI_ERR | 0x22)
#define EFI_HTTP_ERROR (EFI_ERR | 0x23)
