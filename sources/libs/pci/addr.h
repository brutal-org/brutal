#pragma once

#include <brutal/base.h>

typedef struct
{
    uint16_t seg;
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
} PciAddr;

#define PCI_BUS_SHIFT (20)
#define PCI_SLOT_SHIFT (15)
#define PCI_FUNC_SHIFT (12)
