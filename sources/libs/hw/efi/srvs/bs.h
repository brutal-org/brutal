#pragma once

#include <efi/protos/dpp.h>
#include <efi/types.h>

/* Event Types */
#define EVT_TIMER 0x80000000
#define EVT_RUNTIME 0x40000000
#define EVT_NOTIFY_WAIT 0x100
#define EVT_NOTIFY_SIGNAL 0x200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES (EVT_NOTIFY_SIGNAL | 0x1)
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE (EVT_NOTIFY_SIGNAL | 0x2)

#define TPL_APPLICATION 4
#define TPL_CALLBACK 8
#define TPL_NOTIFY 16
#define TPL_HIGH_LEVEL 31

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL (1 << 0)
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL (1 << 1)
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL (1 << 2)
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER (1 << 3)
#define EFI_OPEN_PROTOCOL_BY_DRIVER (1 << 4)
#define EFI_OPEN_PROTOCOL_EXCLUSIVE (1 << 5)

#define EFI_RESERVED_MEMORY_TYPE 0x0
#define EFI_LOADER_CODE 0x1
#define EFI_LOADER_DATA 0x2
#define EFI_BOOT_SERVICES_CODE 0x3
#define EFI_BOOT_SERVICES_DATA 0x4
#define EFI_RUNTIME_SERVICES_CODE 0x5
#define EFI_RUNTIME_SERVICES_DATA 0x6
#define EFI_CONVENTIONAL_MEMORY 0x7
#define EFI_UNUSABLE_MEMORY 0x8
#define EFI_ACPI_RECLAIM_MEMORY 0x9
#define EFI_ACPI_MEMORY_NVS 0xa
#define EFI_MEMORY_MAPPED_IO 0xb
#define EFI_MEMORY_MAPPED_IO_PORT_SPACE 0xc
#define EFI_PAL_CODE 0xd
#define EFI_PERSISTENT_MEMORY 0xe

#define EFI_MAX_MEMORY_TYPE 0xe

#define EFI_USER_KERNEL_MEMORY (0x80000ff0)

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
    uint32_t attributes;
    uint32_t open_count;
} EFIOpenProtocolInformationEntry;

#define DEF_EFI_FUNC(name, ...) typedef EfiStatus (*EFI_##name)(__VA_ARGS__)

DEF_EFI_FUNC(RAISE_TPL, uint64_t);
DEF_EFI_FUNC(RESTORE_TPL, uint64_t);
DEF_EFI_FUNC(ALLOCATE_PAGES, uint64_t, uint64_t, uint64_t, uint64_t *);
DEF_EFI_FUNC(FREE_PAGES, uint64_t, uint64_t);
DEF_EFI_FUNC(GET_MEMORY_MAP, uint64_t *, EFIMemoryDescriptor *, uint64_t *, uint64_t *, uint32_t *);
DEF_EFI_FUNC(ALLOCATE_POOL, uint64_t, uint64_t, void **);
DEF_EFI_FUNC(FREE_POOL, void *);
DEF_EFI_FUNC(CREATE_EVENT, uint32_t, uint64_t, EFIEventNotifyFunction, void *, EfiGuid *, EFIEvent *);
DEF_EFI_FUNC(SET_TIMER, EFIEvent, EFITimerDelay, uint64_t);
DEF_EFI_FUNC(wait_for$_EVENT, uint64_t, EFIEvent *, uint64_t *);
DEF_EFI_FUNC(SIGNAL_EVENT, EFIEvent);
DEF_EFI_FUNC(CLOSE_EVENT, EFIEvent);
DEF_EFI_FUNC(CHECK_EVENT, EFIEvent);
DEF_EFI_FUNC(INSTALL_PROTOCOL_INTERFACE, EFIHandle *, EfiGuid *, EFIInterfaceType, void *);
DEF_EFI_FUNC(REINSTALL_PROTOCOL_INTERFACE, EFIHandle, EfiGuid *, void *, void *);
DEF_EFI_FUNC(UNINSTALL_PROTOCOL_INTERFACE, EFIHandle, EfiGuid *, void *);
DEF_EFI_FUNC(HANDLE_PROTOCOL, EFIHandle, EfiGuid *, void **);
DEF_EFI_FUNC(REGISTER_PROTOCOL_NOTIFY, EfiGuid *, EFIEvent, void **);
DEF_EFI_FUNC(LOCATE_HANDLE, EFILocateSearchType, EfiGuid *, void *, uint64_t *, EFIHandle);
DEF_EFI_FUNC(LOCATE_DEVICE_PATH, EfiGuid *, EFIDevicePath **, EFIHandle);
DEF_EFI_FUNC(INSTALL_CONFIGURATION_TABLE, EfiGuid *, void *);
DEF_EFI_FUNC(IMAGE_LOAD, bool, EFIHandle, EFIDevicePath *, void *, uint64_t, EFIHandle *);
DEF_EFI_FUNC(IMAGE_START, EFIHandle, uint64_t *, uint16_t **);
DEF_EFI_FUNC(EXIT, EFIHandle, EfiStatus, uint64_t, uint16_t *);
DEF_EFI_FUNC(IMAGE_UNLOAD, EFIHandle);
DEF_EFI_FUNC(EXIT_BOOT_SERVICES, EFIHandle, uint64_t);
DEF_EFI_FUNC(GET_NEXT_MONOTONIC_COUNT, uint64_t *);
DEF_EFI_FUNC(STALL, uint64_t);
DEF_EFI_FUNC(SET_WATCHDOG_TIMER, uint64_t, uint64_t, uint64_t, uint16_t *);
DEF_EFI_FUNC(CONNECT_CONTROLLER, EFIHandle, EFIHandle *, EFIDevicePath *, bool);
DEF_EFI_FUNC(DISCONNECT_CONTROLLER, EFIHandle, EFIHandle, EFIHandle);
DEF_EFI_FUNC(OPEN_PROTOCOL, EFIHandle, EfiGuid *, void **, EFIHandle, EFIHandle, uint32_t);
DEF_EFI_FUNC(CLOSE_PROTOCOL, EFIHandle, EfiGuid *, EFIHandle, EFIHandle);
DEF_EFI_FUNC(OPEN_PROTOCOL_INFORMATION, EFIHandle, EfiGuid *, EFIOpenProtocolInformationEntry **, uint64_t *);
DEF_EFI_FUNC(PROTOCOLS_PER_HANDLE, EFIHandle, EfiGuid **, uint64_t *);
DEF_EFI_FUNC(LOCATE_HANDLE_BUFFER, EFILocateSearchType, EfiGuid *, void *, uint64_t *, EFIHandle **);
DEF_EFI_FUNC(LOCATE_PROTOCOL, EfiGuid *, void *, void **);
DEF_EFI_FUNC(INSTALL_MULTIPLE_PROTOCOL_INTERFACES, EFIHandle *, ...);
DEF_EFI_FUNC(UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES, EFIHandle *, ...);
DEF_EFI_FUNC(CALCULATE_CRC32, void *, uint64_t, uint32_t *);
DEF_EFI_FUNC(COPY_MEM, void *, void *, uint64_t);
DEF_EFI_FUNC(SET_MEM, void *, uint64_t, uint8_t);
DEF_EFI_FUNC(CREATE_EVENT_EX, uint32_t, uint64_t, EFIEventNotifyFunction, void *, EfiGuid *, EFIEvent *);

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
    EFI_wait_for$_EVENT wait_for_event;
    EFI_SIGNAL_EVENT signal_event;
    EFI_CLOSE_EVENT close_event;
    EFI_CHECK_EVENT check_event;
    EFI_INSTALL_PROTOCOL_INTERFACE install_protocol_interface;
    EFI_REINSTALL_PROTOCOL_INTERFACE reinstall_protocol_interface;
    EFI_UNINSTALL_PROTOCOL_INTERFACE uninstall_protocol_interface;
    EFI_HANDLE_PROTOCOL handle_protocol;
    void *_reserved;
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
