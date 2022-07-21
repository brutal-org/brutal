#pragma once

#include <brutal/base.h>

/* from https://datatracker.ietf.org/doc/html/rfc791 */ 
typedef struct PACKED
{
    uint8_t version : 4;
    uint8_t internet_header_length : 4;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags : 3;
    uint16_t fragment_offset : 13;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t source_addr;
    uint32_t destination_addr;
} Ipv4Header;

typedef struct PACKED
{
    Ipv4Header header;
    uint8_t data[];
} Ipv4;

/* from: https://datatracker.ietf.org/doc/html/rfc8200 */
typedef struct PACKED
{
    uint32_t version : 4;
    uint32_t traffic_class : 8;
    uint32_t flow_label : 20;
    uint16_t payload_length;
    uint8_t next_header;
    uint8_t hop_limit;
    uint8_t source_addr[32];
    uint8_t destination_addr[32];
} Ipv6Header;

typedef struct PACKED
{
    Ipv6Header header;
    uint8_t data[];
} Ipv6;

#define IPV4_ANY 0xFFFFFFFF
