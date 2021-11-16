#pragma once

#include <brutal/base.h>
#include <bal/abi.h>
#include "pci.h"

typedef struct
{
    BalIo io;
    bool has_eeprom;
    uint8_t mac[6];
} E1000Device;
