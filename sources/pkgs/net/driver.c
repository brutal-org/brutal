#include "driver.h"

NicDevice nic_device_list[] = {
    {VENDOR_REALTEK_SEMI, DEVID_RTL8100, &rtl8139_driver},
    {0x0, 0x0, NULL}
};
