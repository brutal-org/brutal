#pragma once

#include <brutal/base.h>
#include <bal/abi.h>
#include "pci.h"

/*
 * datasheet: https://www.mouser.com/datasheet/2/612/i217-ethernet-controller-datasheet-257741.pdf
*/
#define E1000_EEPROM_REG 0x14 /* XXX: marked as reserved in i217 datasheet (i217 doesn't have eeprom) */
#define E1000_RAL_LOW_REG 0x05400 /* lower 32bit of the 48bit eth addr */
#define E1000_RAL_HIGH_REG 0x05404 /* upper 16 bits of the 48bit eth addr */

typedef struct
{
    BalIo io;
    bool has_eeprom;
    uint8_t mac[6];
} E1000Device;
