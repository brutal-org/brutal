#pragma once

#include <brutal/base.h>

typedef struct
{
    uint16_t seg;
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
} PciAddr;
