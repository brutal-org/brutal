#pragma once

#include <acpi/base.h>
#include <pci/addr.h>
#include <pci/capabilities.h>
#include <pci/config.h>
#include <pci/group.h>
#include <protos/pci.h>
#include <brutal-ds>

typedef struct
{
    Vec(PciGroup) groups;
} Pci;

void pci_init(Pci *pci, Acpi *acpi, Alloc *alloc);

void pci_deinit(Pci *pci);

Iter pci_iter(Pci *pci, IterFn fn, void *ctx);

PciConfig *pci_config(Pci *pci, PciAddr addr);

PciBarInfo pci_read_bar(Pci *pci, PciAddr addr, int bar);

bool pci_bind_msi(uint8_t cpu, uint8_t vector, PciCapability *pci_msi_cap);
