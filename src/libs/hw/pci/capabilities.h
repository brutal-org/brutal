#pragma once

#include <brutal-text>

#define MSI_CTRL_ENABLE (1 << 0)
#define MSI_CTRL_64_BIT (1 << 7)
#define MSI_CTRL_MME_OFF (4)

typedef struct
{
    uint16_t control;
    uint64_t address;
    uint16_t data;
    uint16_t _reserved;
    uint32_t mask;
    uint32_t pending;
} PciMSI;

typedef enum
{
    PCI_CAP_POWER_MANAGEMENT = 0x1,
    PCI_CAP_MSI = 0x5,
    PCI_CAP_VENDOR_SPECIFIC = 0x9,
    PCI_CAP_PCIE = 0x10,
    PCI_CAP_MSIX = 0x11,
    PCI_CAP_SATA = 0x12,
} PciCapabilities;

typedef struct PACKED
{

    uint8_t cap_id;
    uint8_t next;

    union
    {
        PciMSI msi;
    };
} PciCapability;

Str cap2str(uint8_t id);
