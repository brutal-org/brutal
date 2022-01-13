#pragma once

#include <brutal-base>
#include <bal/abi.h>
#include "pci.h"

/*
 * intel i217 datasheet https://www.mouser.com/datasheet/2/612/i217-ethernet-controller-datasheet-257741.pdf
 * intel 82540EM datasheet https://www.intel.com/content/dam/doc/manual/pci-pci-x-family-gbe-controllers-software-dev-manual.pdf
*/
#define E1000_STATUS_REG 0x8
#define E1000_EEPROM_REG 0x14 /* XXX: marked as reserved in i217 datasheet (i217 doesn't have eeprom) */
#define E1000_RAL_LOW_REG 0x05400 /* lower 32bit of the 48bit eth addr */
#define E1000_RAL_HIGH_REG 0x05404 /* upper 16 bits of the 48bit eth addr */

#define E1000_END_REG 0x0C5FC

typedef struct
{
    BalIo io;
    BalMem mmio;
    bool has_eeprom;
    uint16_t int_line;
    uint8_t mac[6];
} E1000Device;
