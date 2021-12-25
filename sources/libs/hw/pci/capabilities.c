#include <hw/pci/capabilities.h>

const char *caps[] =
    {
        [PCI_CAP_POWER_MANAGEMENT] = "power management",
        [PCI_CAP_VENDOR_SPECIFIC] = "Vendor Specific",
        [PCI_CAP_MSI] = "MSI",
        [PCI_CAP_PCIE] = "PCIe",
        [PCI_CAP_MSIX] = "MSIx",
        [PCI_CAP_SATA] = "SATA",
};
Str cap2str(uint8_t id)
{
    return str$(caps[id]);
}
