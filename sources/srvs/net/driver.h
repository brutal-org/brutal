#pragma once

#include <brutal-base>
#include <stdint.h>
#include <pci/pci.h>

/* vendors ------------------------------------------------------------------ */

#define VENDOR_REALTEK_SEMI 0x10EC
#define VENDOR_INTEL 0x8086

/* devices ID --------------------------------------------------------------- */

#define DEVID_RTL8100 0x8139 /* RTL-8100/8101L/8139 PCI Fast Eth Adapter */
#define DEVID_E1000_QEMU 0x100E
#define DEVID_E1000_82544GC 0x100C
#define DEVID_E1000_82545EM 0x100F
#define DEVID_E1000_82574L 0x10d3

/* nic drivers abstraction -------------------------------------------------- */

typedef struct {
    void *(*init)(PciConfigType0 *, uint16_t);
    void (*deinit)(void *);
    void (*handle)(void *);
    void (*send)(void *, void *, size_t);
    uint8_t *(*get_mac)(void *);
} NicDriver;

typedef struct{
    uint16_t vendor;
    uint16_t device;
    void *driver;
} NicDevice;

extern NicDriver rtl8139_driver;
extern NicDriver e1000_driver;

extern NicDevice nic_device_list[];
