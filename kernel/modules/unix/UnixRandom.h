#pragma once

#include <libmath/Random.h>

#include "unix/UNIXDevice.h"

struct UnixRandom : public UNIXDevice
{
private:
    Math::Random _random{};

public:
    UnixRandom(DeviceAddress address) : UNIXDevice(address, DeviceClass::RANDOM)
    {
    }

    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size) override
    {
        UNUSED(offset);

        for (size_t i = 0; i < size; i++)
        {
            ((char *)buffer)[i] = _random.next_u8();
        }

        return size;
    }

    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override
    {
        UNUSED(offset);
        UNUSED(buffer);

        return size;
    }
};
