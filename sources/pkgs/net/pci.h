#pragma once

#define PCI_VENDOR_REALTEK_SEMI 0x10EC
#define PCI_DEVID_RTL8139 0x8139 

#define PCI_BAR_IO 0x1

#include <pci/pci.h>
#include <pci/config.h>
#include <bal/boot/handover.h>

void pci_get_network_cards(Handover *handover);
uint32_t pci_get_io_base(PciConfigType0 *conf);
