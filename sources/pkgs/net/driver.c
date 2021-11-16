#include "driver.h"

NicDevice nic_device_list[] = {
    {VENDOR_REALTEK_SEMI, DEVID_RTL8100, &rtl8139_driver},
    {VENDOR_INTEL, DEVID_E1000_QEMU, &e1000_driver},
    {0x0, 0x0, NULL}
};
