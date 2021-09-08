#pragma once

#include <pci/addr.h>
#include <pci/config.h>
#include <syscalls/mmio.h>

typedef struct
{
    uint16_t seg;

    uint8_t bus_start;
    uint8_t bus_end;

    BrMmio mmio;
} PciGroup;

PciConfig *pci_group_config(PciGroup *group, PciAddr addr);
