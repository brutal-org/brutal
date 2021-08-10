#pragma once

#include <efi/base.h>
#include <efi/dpp.h>

/* Event Types */
#define EVT_TIMER 0x80000000
#define EVT_RUNTIME 0x40000000
#define EVT_NOTIFY_WAIT 0x00000100
#define EVT_NOTIFY_SIGNAL 0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES 0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x00000202

#define TPL_APPLICATION 4
#define TPL_CALLBACK 8
#define TPL_NOTIFY 16
#define TPL_HIGH_LEVEL 31

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

#define EFI_RESERVED_MEMORY_TYPE 0x00000000
#define EFI_LOADER_CODE 0x00000001
#define EFI_LOADER_DATA 0x00000002
#define EFI_BOOT_SERVICES_CODE 0x00000003
#define EFI_BOOT_SERVICES_DATA 0x00000004
#define EFI_RUNTIME_SERVICES_CODE 0x00000005
#define EFI_RUNTIME_SERVICES_DATA 0x00000006
#define EFI_CONVENTIONAL_MEMORY 0x00000007
#define EFI_UNUSABLE_MEMORY 0x00000008
#define EFI_ACPI_RECLAIM_MEMORY 0x00000009
#define EFI_ACPI_MEMORY_NVS 0x0000000a
#define EFI_MEMORY_MAPPED_IO 0x0000000b
#define EFI_MEMORY_MAPPED_IO_PORT_SPACE 0x0000000c
#define EFI_PAL_CODE 0x0000000d
#define EFI_PERSISTENT_MEMORY 0x0000000e
#define EFI_MAX_MEMORY_TYPE 0x0000000e

typedef void (*EFIEventNotifyFunction)(EFIEvent, void *);

typedef enum
{
    timer_cncel,
    timer_periodic,
    timer_relative
} EFITimerDelay;

typedef enum
{
    ALLOCATE_ANY_PAGES,
    ALLOCATE_MAX_ADDRESS,
    ALLOCATE_ADDRESS,
    MAX_ALLOCATE_TYPE
} EFIAllocateType;

typedef enum
{
    EFI_NATIVE_INTERFACE
} EFIInterfaceType;

typedef enum
{
    ALL_HANDLES,
    BY_REGISTER_NOTIFY,
    BY_PROTOCOL
} EFILocateSearchType;

typedef struct EFI_OPEN_PROTOCOL_INFORMATION_ENTRY
{
    EFIHandle agent_handle;
    EFIHandle controller_handle;
    u32 attributes;
    u32 open_count;
} EFIOpenProtocolInformationEntry;

#define DEF_EFI_FUNC(name, ...) typedef EFIStatus (*EFI_##name)(__VA_ARGS__)

DEF_EFI_FUNC(RAISE_TPL, u64);
DEF_EFI_FUNC(RESTORE_TPL, u64);
DEF_EFI_FUNC(ALLOCATE_PAGES, EFIAllocateType, u64, u64, u64 *);
DEF_EFI_FUNC(FREE_PAGES, u64, u64);
DEF_EFI_FUNC(GET_MEMORY_MAP, u64 *, EFIMemoryDescriptor *, u64 *, u64 *, u32 *);
DEF_EFI_FUNC(ALLOCATE_POOL, u64, u64, void **);
DEF_EFI_FUNC(FREE_POOL, void *);
DEF_EFI_FUNC(CREATE_EVENT, u32, u64, EFIEventNotifyFunction, void *, EFIGUID *, EFIEvent *);
DEF_EFI_FUNC(SET_TIMER, EFIEvent, EFITimerDelay, u64);
DEF_EFI_FUNC(WAIT_FOR_EVENT, u64, EFIEvent *, u64 *);
DEF_EFI_FUNC(SIGNAL_EVENT, EFIEvent);
DEF_EFI_FUNC(CLOSE_EVENT, EFIEvent);
DEF_EFI_FUNC(CHECK_EVENT, EFIEvent);
DEF_EFI_FUNC(INSTALL_PROTOCOL_INTERFACE, EFIHandle *, EFIGUID *, EFIInterfaceType, void *);
DEF_EFI_FUNC(REINSTALL_PROTOCOL_INTERFACE, EFIHandle, EFIGUID *, void *, void *);
DEF_EFI_FUNC(UNINSTALL_PROTOCOL_INTERFACE, EFIHandle, EFIGUID *, void *);
DEF_EFI_FUNC(HANDLE_PROTOCOL, EFIHandle, EFIGUID *, void **);
DEF_EFI_FUNC(REGISTER_PROTOCOL_NOTIFY, EFIGUID *, EFIEvent, void **);
DEF_EFI_FUNC(LOCATE_HANDLE, EFILocateSearchType, EFIGUID *, void *, u64 *, EFIHandle);
DEF_EFI_FUNC(LOCATE_DEVICE_PATH, EFIGUID *, EFIDevicePath **, EFIHandle);
DEF_EFI_FUNC(INSTALL_CONFIGURATION_TABLE, EFIGUID *, void *);
DEF_EFI_FUNC(IMAGE_LOAD, bool, EFIHandle, EFIDevicePath *, void *, u64, EFIHandle *);
DEF_EFI_FUNC(IMAGE_START, EFIHandle, u64 *, char16 **);
DEF_EFI_FUNC(EXIT, EFIHandle, EFIStatus, u64, char16 *);
DEF_EFI_FUNC(IMAGE_UNLOAD, EFIHandle);
DEF_EFI_FUNC(EXIT_BOOT_SERVICES, EFIHandle, u64);
DEF_EFI_FUNC(GET_NEXT_MONOTONIC_COUNT, u64 *);
DEF_EFI_FUNC(STALL, u64);
DEF_EFI_FUNC(SET_WATCHDOG_TIMER, u64, u64, u64, char16 *);
DEF_EFI_FUNC(CONNECT_CONTROLLER, EFIHandle, EFIHandle *, EFIDevicePath *, bool);
DEF_EFI_FUNC(DISCONNECT_CONTROLLER, EFIHandle, EFIHandle, EFIHandle);
DEF_EFI_FUNC(OPEN_PROTOCOL, EFIHandle, EFIGUID *, void **, EFIHandle, EFIHandle, u32);
DEF_EFI_FUNC(CLOSE_PROTOCOL, EFIHandle, EFIGUID *, EFIHandle, EFIHandle);
DEF_EFI_FUNC(OPEN_PROTOCOL_INFORMATION, EFIHandle, EFIGUID *, EFIOpenProtocolInformationEntry **, u64 *);
DEF_EFI_FUNC(PROTOCOLS_PER_HANDLE, EFIHandle, EFIGUID **, u64 *);
DEF_EFI_FUNC(LOCATE_HANDLE_BUFFER, EFILocateSearchType, EFIGUID *, void *, u64 *, EFIHandle **);
DEF_EFI_FUNC(LOCATE_PROTOCOL, EFIGUID *, void *, void **);
DEF_EFI_FUNC(INSTALL_MULTIPLE_PROTOCOL_INTERFACES, EFIHandle *, ...);
DEF_EFI_FUNC(UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES, EFIHandle *, ...);
DEF_EFI_FUNC(CALCULATE_CRC32, void *, u64, u32 *);
DEF_EFI_FUNC(COPY_MEM, void *, void *, u64);
DEF_EFI_FUNC(SET_MEM, void *, u64, u8);
DEF_EFI_FUNC(CREATE_EVENT_EX, u32, u64, EFIEventNotifyFunction, void *, EFIGUID *, EFIEvent *);

typedef struct
{
    EFITableHeader hdr;
    EFI_RAISE_TPL raise_tpl;
    EFI_RESTORE_TPL restore_tpl;
    EFI_ALLOCATE_PAGES allocate_pages;
    EFI_FREE_PAGES free_pages;
    EFI_GET_MEMORY_MAP get_memory_map;
    EFI_ALLOCATE_POOL allocate_pool;
    EFI_FREE_POOL free_pool;
    EFI_CREATE_EVENT create_event;
    EFI_SET_TIMER set_timer;
    EFI_WAIT_FOR_EVENT wait_for_event;
    EFI_SIGNAL_EVENT signal_event;
    EFI_CLOSE_EVENT close_event;
    EFI_CHECK_EVENT check_event;
    EFI_INSTALL_PROTOCOL_INTERFACE install_protocol_interface;
    EFI_REINSTALL_PROTOCOL_INTERFACE reinstall_protocol_interface;
    EFI_UNINSTALL_PROTOCOL_INTERFACE uninstall_protocol_interface;
    EFI_HANDLE_PROTOCOL handle_protocol;
    void *reserved;
    EFI_REGISTER_PROTOCOL_NOTIFY register_protocol_notify;
    EFI_LOCATE_HANDLE locate_handle;
    EFI_LOCATE_DEVICE_PATH locate_device_path;
    EFI_INSTALL_CONFIGURATION_TABLE install_configuration_table;
    EFI_IMAGE_LOAD load_image;
    EFI_IMAGE_START start_image;
    EFI_EXIT exit;
    EFI_IMAGE_UNLOAD unload_image;
    EFI_EXIT_BOOT_SERVICES exit_boot_services;
    EFI_GET_NEXT_MONOTONIC_COUNT get_next_monotonic_count;
    EFI_STALL stall;
    EFI_SET_WATCHDOG_TIMER set_watchdog_timer;
    EFI_CONNECT_CONTROLLER connect_controller;
    EFI_DISCONNECT_CONTROLLER disconnect_controller;
    EFI_OPEN_PROTOCOL open_protocol;
    EFI_CLOSE_PROTOCOL close_protocol;
    EFI_OPEN_PROTOCOL_INFORMATION open_protocol_information;
    EFI_PROTOCOLS_PER_HANDLE protocols_per_handle;
    EFI_LOCATE_HANDLE_BUFFER locate_handle_buffer;
    EFI_LOCATE_PROTOCOL locate_protocol;
    EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES install_multiple_protocol_interfaces;
    EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES uninstall_multiple_protocol_interfaces;
    EFI_CALCULATE_CRC32 calculate_crc32;
    EFI_COPY_MEM copy_mem;
    EFI_SET_MEM set_mem;
    EFI_CREATE_EVENT_EX create_event_ex;
} EFIBootServices;
