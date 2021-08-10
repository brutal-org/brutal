#pragma once
#include <efi/base.h>
#include <efi/bs.h>

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
    u64 length;
    union
    {
        u64 data_block;
        u64 continuation_pointer;
    } _union;

} EFICapsuleBlockDescriptor;

typedef struct
{
    EFIGUID capsule_guid;
    u32 header_size;
    u32 flags;
    u32 capsule_image_size;
} EFICapsuleHeader;

DEF_EFI_FUNC(GET_TIME, EFITime *, EFITimeCapabilities *);
DEF_EFI_FUNC(SET_TIME, EFITime *);
DEF_EFI_FUNC(GET_WAKEUP_TIME, bool *, bool *, EFITime *);
DEF_EFI_FUNC(SET_WAKEUP_TIME, bool, EFITime *);
DEF_EFI_FUNC(SET_VIRTUAL_ADDRESS_MAP, u64, u64, u32, EFIMemoryDescriptor *);
DEF_EFI_FUNC(CONVERT_POINTER, u64, void **);
DEF_EFI_FUNC(GET_VARIABLE, char16 *, EFIGUID *, u32 *, u64 *, void *);
DEF_EFI_FUNC(GET_NEXT_VARIABLE_NAME, u64, char16 *, EFIGUID *);
DEF_EFI_FUNC(SET_VARIABLE, char16 *, EFIGUID *, u32, u64, void *);
DEF_EFI_FUNC(GET_NEXT_HIGH_MONO_COUNT, u32 *);
DEF_EFI_FUNC(RESET_SYSTEM, EFIResetType, EFIStatus, u64, void *);
DEF_EFI_FUNC(UPDATE_CAPSULE, EFICapsuleHeader **, u64, u64);
DEF_EFI_FUNC(QUERY_CAPSULE_CAPABILITIES, EFICapsuleHeader **, u64, u64 *, EFIResetType *);
DEF_EFI_FUNC(QUERY_VARIABLE_INFO, u32, u64 *, u64 *, u64 *);

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
