#pragma once

#include <acpi/stdh.h>

#define ACPI_HPET_SIG str$("HPET")

typedef struct PACKED
{
    AcpiSdth stdh;

    uint8_t hardware_rev_id;
    uint8_t info;
    uint16_t pci_vendor_id;
    uint8_t address_space_id;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved1;
    uint64_t address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} AcpiHpet;
