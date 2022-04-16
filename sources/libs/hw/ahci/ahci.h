#pragma once

#include <ahci/device.h>
#include <ahci/hba.h>
#include <pci/pci.h>
#include <brutal-ds>

typedef struct
{
    PciBarInfo pci_bar;
    volatile HbaMemory *hba_mem;
    Vec(AhciDevice) devs;
} Ahci;

Str ahci_port_sig_to_str(HbaPortSig type);

void ahci_init(Ahci *ahci, PciBarInfo *bar, Alloc *alloc);
