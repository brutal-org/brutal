#pragma once

#include <acpi/base.h>
#include <brutal/ds.h>
#include <pci/addr.h>
#include <pci/capabilities.h>
#include <pci/config.h>
#include <pci/group.h>
#include <protos/hw/pci.h>

typedef struct
{
    Vec(PciGroup) groups;
} Pci;

void pci_init(Pci *pci, Acpi *acpi, Alloc *alloc);

void pci_deinit(Pci *pci);

Iter pci_iter(Pci *pci, IterFn fn, void *ctx);

PciConfig *pci_config(Pci *pci, PciAddr addr);

PciBarInfo pci_get_bar(Pci *pci, PciAddr addr, int bar);

bool pci_set_msi(uint8_t cpu, uint8_t vector, PciCapability *pci_msi_cap);
