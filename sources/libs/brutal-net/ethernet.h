#pragma once

#include <brutal-base>
#include <brutal-net/arp.h>
#include <brutal-net/ip.h>

#define ETHERNET_ETHERTYPE_IPV4 0x0800
#define ETHERNET_ETHERTYPE_ARP  0x0806
#define ETHERNET_ETHERTYPE_IPV6 0x86DD

#define ETHERNET_PAYLOAD_MAX_SIZE 1500

typedef struct PACKED
{
    uint8_t mac_dst[6];
    uint8_t mac_src[6];
    be_uint16_t ethertype;
} EthernetFrameHeader;

typedef struct PACKED
{
    uint8_t *frame;
    size_t size;
} EthernetFrame;

EthernetFrame ethernet_make_frame(EthernetFrameHeader hdr, uint8_t *payload, size_t payload_size);
