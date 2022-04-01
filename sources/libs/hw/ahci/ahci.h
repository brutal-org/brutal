#pragma once

#include <ahci/device.h>
#include <ahci/hba.h>
#include <bruwutal/ds.h>
#include <pci/pci.h>

typedef struct
{
    PciBarInfo pci_bar;
    volatile HbaMemory *hba_mem;
    Vec(AhciDevice) devs;
} Ahci;

Str ahci_port_sig_to_str(HbaPortSig type);

void ahci_init(Ahci *ahci, PciBarInfo *bar, Alloc *alloc);
