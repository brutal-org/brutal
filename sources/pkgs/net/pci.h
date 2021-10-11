#pragma once

#define PCI_VENDOR_REALTEK_SEMI 0x10EC
#define PCI_DEVID_RTL8139 0x8139 

#define PCI_BAR_IO 0x1

#include <pci/pci.h>
#include <pci/config.h>
#include <handover/handover.h>

void pci_get_network_cards(Handover *handover);
