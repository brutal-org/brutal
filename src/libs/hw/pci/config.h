#pragma once

#include <brutal-base>

typedef struct PACKED
{
    uint16_t vendor;
    uint16_t device;
    uint16_t command;
    uint16_t status;
    uint8_t revison;
    uint8_t progif;
    uint8_t subclass;
    uint8_t class;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist; // unused
} PciConfig;

typedef struct PACKED
{
    PciConfig base;

    uint32_t bars[6];
    uint32_t cardbus_cis_pointer;
    uint16_t subsystem_vendor;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base_address;
    uint8_t capabilities_pointer;
    uint8_t reserved0[3];
    uint8_t reserved1[4];
    uint8_t interrupt_line;
    uint8_t interrupt_pint;
    uint8_t min_grant;
    uint8_t max_latency;
} PciConfigType0;

typedef struct PACKED
{
    PciConfig base;

    uint32_t bars[2];
    uint8_t primary_bus_number;
    uint8_t secondary_bus_number;
    uint8_t subordinate_bus_number;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    uint16_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_memory_base_upper32;
    uint32_t prefetchable_memory_limit_upper32;
    uint16_t io_base_upper32;
    uint16_t io_limit_upper32;
    uint8_t capability_pointer;
    uint8_t reserved[3];
    uint32_t expansion_rom_base_address;
    uint8_t interrupt_line;
    uint8_t interrupt_pint;
    uint16_t bridge_control;
} PciConfigType1;
