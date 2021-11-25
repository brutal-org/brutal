#pragma once

#include <bal/hw.h>
#include <pci/addr.h>
#include <pci/config.h>

typedef struct
{
    uint16_t seg;

    uint8_t bus_start;
    uint8_t bus_end;

    BalMem mem;
} PciGroup;

PciConfig *pci_group_config(PciGroup *group, PciAddr addr);
