#pragma once

#include "system/devices/Driver.h"

#include "ps2/Legacy.h"

struct LegacyDevice : public Device
{
private:
public:
    LegacyAddress legacy_address() { return address().legacy(); }

    LegacyDevice(DeviceAddress address, DeviceClass klass)
        : Device(address, klass)
    {
    }

    int interrupt() override
    {
        switch (legacy_address())
        {

        case LEGACY_KEYBOARD:
            return 1;

        case LEGACY_COM2:
        case LEGACY_COM4:
            return 3;

        case LEGACY_COM1:
        case LEGACY_COM3:
            return 4;

        case LEGACY_ATA0:
        case LEGACY_ATA1:
            return 5;

        case LEGACY_ATA2:
        case LEGACY_ATA3:
            return 6;

        case LEGACY_MOUSE:
            return 12;

        default:
            break;
        }

        ASSERT_NOT_REACHED();
    }
};

template <typename LegacyDeviceType>
struct LegacyDeviceMatcher : public DeviceMatcher
{
private:
    LegacyAddress _address;

public:
    LegacyDeviceMatcher(const char *name, LegacyAddress address)
        : DeviceMatcher(BUS_LEGACY, name),
          _address(address)
    {
    }

    bool match(DeviceAddress address)
    {
        return _address == address.legacy();
    }

    Ref<Device> instance(DeviceAddress address)
    {
        return make<LegacyDeviceType>(address);
    }
};
