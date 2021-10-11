#pragma once

#include <brutal/base.h>

#define ARP_PROTOCOL_ETHERNET 0x1

#define ARP_OP_REQUEST 0x1
#define ARP_OP_REPLY 0x2

typedef struct PACKED
{
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_addr_len; // ethernet = 6
    uint8_t protocol_addr_len; // ipv4 = 4
    uint16_t opcode;
    uint8_t src_hardware_addr[6];
    uint8_t src_protocol_addr[4];
    uint8_t dest_hardware_addr[6];
    uint8_t dest_protocol_addr[4];
} ArpHeader;

typedef struct PACKED
{
    ArpHeader header;
    uint8_t data[];
} Arp;
