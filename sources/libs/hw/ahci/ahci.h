#pragma once

#include <ahci/device.h>
#include <ahci/hba.h>
#include <brutal/ds.h>
#include <pci/pci.h>

typedef struct
{
    PciBarInfo pci_bar;
    volatile HbaMemory *hba_mem;
    Vec(AhciDevice) devs;
} Ahci;

typedef enum
{
    AHCI_DEVICE_NONE,
    AHCI_DEVICE_SATA,
    AHCI_DEVICE_SATAPI,
    AHCI_DEVICE_SEMB,
    AHCI_DEVICE_PM,
    AHCI_DEVICE_UNCONNECTED,
} AhciDeviceType;

void ahci_init(Ahci *ahci, PciBarInfo *bar, Alloc *alloc);
