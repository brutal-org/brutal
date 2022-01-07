#pragma once

#include <efi/srvs/bs.h>
#include <efi/types.h>

#define EFI_OPTIONAL_POINTER 0x00000001

#define CAPSULE_FLAGS_PERSIST_ACROSS_RESET 0x00010000
#define CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE 0x00020000
#define CAPSULE_FLAGS_INITIATE_RESET 0x00040000

typedef enum
{
    EFI_RESET_COLD,
    EFI_RESET_WARM,
    EFI_RESET_SHUTDOWN,
    EFI_RESET_PLATFORM_SPECIFIC
} EFIResetType;

typedef struct
{
    uint64_t len;
    union
    {
        uint64_t data_block;
        uint64_t continuation_pointer;
    } _union;

} EFICapsuleBlockDescriptor;

typedef struct
{
    EfiGuid capsule_guid;
    uint32_t header_size;
    uint32_t flags;
    uint32_t capsule_image_size;
} EFICapsuleHeader;

DEF_EFI_FUNC(GET_TIME, EFITime *, EFITimeCapabilities *);
DEF_EFI_FUNC(SET_TIME, EFITime *);
DEF_EFI_FUNC(GET_WAKEUP_TIME, bool *, bool *, EFITime *);
DEF_EFI_FUNC(SET_WAKEUP_TIME, bool, EFITime *);
DEF_EFI_FUNC(SET_VIRTUAL_ADDRESS_MAP, uint64_t, uint64_t, uint32_t, EFIMemoryDescriptor *);
DEF_EFI_FUNC(CONVERT_POINTER, uint64_t, void **);
DEF_EFI_FUNC(GET_VARIABLE, uint16_t *, EfiGuid *, uint32_t *, uint64_t *, void *);
DEF_EFI_FUNC(GET_NEXT_VARIABLE_NAME, uint64_t, uint16_t *, EfiGuid *);
DEF_EFI_FUNC(SET_VARIABLE, uint16_t *, EfiGuid *, uint32_t, uint64_t, void *);
DEF_EFI_FUNC(GET_NEXT_HIGH_MONO_COUNT, uint32_t *);
DEF_EFI_FUNC(RESET_SYSTEM, EFIResetType, EfiStatus, uint64_t, void *);
DEF_EFI_FUNC(UPDATE_CAPSULE, EFICapsuleHeader **, uint64_t, uint64_t);
DEF_EFI_FUNC(QUERY_CAPSULE_CAPABILITIES, EFICapsuleHeader **, uint64_t, uint64_t *, EFIResetType *);
DEF_EFI_FUNC(QUERY_VARIABLE_INFO, uint32_t, uint64_t *, uint64_t *, uint64_t *);

typedef struct
{
    EFITableHeader hdr;
    EFI_GET_TIME get_time;
    EFI_SET_TIME set_time;
    EFI_GET_WAKEUP_TIME get_wakeup_time;
    EFI_SET_WAKEUP_TIME set_wakeup_time;
    EFI_SET_VIRTUAL_ADDRESS_MAP set_virtual_address_map;
    EFI_CONVERT_POINTER convert_pointer;
    EFI_GET_VARIABLE get_variable;
    EFI_GET_NEXT_VARIABLE_NAME get_next_variable_name;
    EFI_SET_VARIABLE set_variable;
    EFI_GET_NEXT_HIGH_MONO_COUNT get_next_high_monotonic_count;
    EFI_RESET_SYSTEM reset_system;
    EFI_UPDATE_CAPSULE update_capsule;
    EFI_QUERY_CAPSULE_CAPABILITIES query_capsule_capabilities;
    EFI_QUERY_VARIABLE_INFO query_variable_info;
} EFIRuntimeServices;
