#pragma once

#include <brutal-base>

#define ARP_HW_TYPE_ETHERNET 0x1
#define ARP_HW_ETHERNET_LEN 0x6

#define ARP_PROTO_TYPE_IP 0x0800
#define ARP_PROTO_IPV4_LEN 0x4
#define ARP_PROTO_IPV6_LEN 0x16

#define ARP_OP_REQUEST 0x1
#define ARP_OP_REPLY 0x2

typedef struct PACKED
{
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t hw_addr_len;
    uint8_t proto_addr_len;
    uint16_t opcode;
    uint8_t src_hw_addr[ARP_HW_ETHERNET_LEN];
    uint8_t src_proto_addr[ARP_PROTO_IPV4_LEN]; /* XXX: todo IPv6 */
    uint8_t dest_hw_addr[ARP_HW_ETHERNET_LEN];
    uint8_t dest_protocol_addr[ARP_PROTO_IPV4_LEN];
} ArpHeader;

typedef struct PACKED
{
    uint8_t *raw;
    size_t size;
} Arp;
