#pragma once

#include <brutal/base.h>
#include "brutal/base/attributes.h"

typedef struct PACKED
{
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} UdpHeader;

typedef struct PACKED
{
    UdpHeader header,
    uint8_t data[];
} Udp;