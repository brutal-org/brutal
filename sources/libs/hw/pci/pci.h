#pragma once

#include <acpi/base.h>
#include <bal/mmio.h>
#include <brutal/ds.h>
#include <pci/addr.h>
#include <pci/config.h>
#include <pci/group.h>

typedef struct
{
    Vec(PciGroup) groups;
} Pci;

void pci_init(Pci *pci, Acpi *acpi, Alloc *alloc);

void pci_deinit(Pci *pci);

Iter pci_iter(Pci *pci, IterFn fn, void *ctx);

PciConfig *pci_config(Pci *pci, PciAddr addr);
