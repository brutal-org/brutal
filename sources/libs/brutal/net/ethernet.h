#pragma once

#include <brutal/base.h>

#define ETHERNET_ETHERTYPE_IPV4 0x0800
#define ETHERNET_ETHERTYPE_ARP  0x0806
#define ETHERNET_ETHERTYPE_IPV6 0x86DD

typedef struct PACKED
{
    uint8_t mac_dst[6];
    uint8_t mac_src[6];
    uint16_t ethertype;
} EthernetFrameHeader;

typedef struct PACKED
{
    EthernetFrameHeader header;
    uint8_t data[];
} EthernetFrame;

EthernetFrame *ethernet_make_frame();