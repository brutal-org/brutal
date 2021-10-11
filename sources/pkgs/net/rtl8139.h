#pragma once

#include <brutal/base.h>
#include <bal/syscalls.h>
#include "pci.h"

/* from: RTL8139D DataSheet p10-11 */
#define RTL8139_IDR0_REG 0x0
#define RTL8139_IDR4_REG 0x4
#define RTL8139_TSD0 0x10
#define RTL8139_TSD1 0x14
#define RTL8139_TSD2 0x18
#define RTL8139_TSD3 0x1C
#define RTL8139_TSAD0 0x20
#define RTL8139_TSAD1 0x24
#define RTL8139_TSAD2 0x28
#define RTL8139_TSAD3 0x2C
#define RTL8139_RBSTART_REG 0x30
#define RTL8139_CR_REG 0x37
#define RTL8139_IMR_REG 0x3C
#define RTL8139_ISR_REG 0x3E
#define RTL8139_RCR_REG 0x44
#define RTL8139_CONFIG1_REG 0x52

#define RTL8139_CR_TE 0x4 /* transmitter enable */
#define RTL8139_CR_RE 0x8 /* receiver enable */

#define RTL8139_IMR_ROK 0x1
#define RTL8139_IMR_RER 0x2
#define RTL8139_IMR_TOK 0x4
#define RTL8139_IMR_TER 0x8

#define RTL8139_ISR_ROK 0x1
#define RTL8139_ISR_RER 0x2
#define RTL8139_ISR_TOK 0x4
#define RTL8139_ISR_TER 0x8

#define RTL8139_RCR_AAP 0x1  /* Accept All Packets */
#define RTL8139_RCR_APM 0x2  /* Accept Physical Match packets */
#define RTL8139_RCR_AM 0x4   /* Accept Multicast packets */
#define RTL8139_RCR_AB 0x8   /* Accept Broadcast packets */
#define RTL8139_RCR_AR 0x10  /* Accept Runt */
#define RTL8139_RCR_AER 0x20 /* Accept Error packet */
#define RTL8139_RCR_WRAP 0x80

#define RTL8139_RCR_ACCEPT_ALL RTL8139_RCR_AAP |     \
                                     RTL8139_RCR_APM | \
                                     RTL8139_RCR_AM |  \
                                     RTL8139_RCR_AB | \
                                     RTL8139_RCR_AR | \
                                     RTL8139_RCR_AER

#define RTL8139_CONFIG1_LWACT 0x10

typedef struct
{
    uint32_t io_base;
    uint8_t mac[6];
    uint8_t *rx_buffer;
    BrHandle irq_handle;
    uint8_t tx_curr;
    uint16_t int_line;
} RTL8139Device;
