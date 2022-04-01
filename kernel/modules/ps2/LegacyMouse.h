#pragma once

#include <abi/Mouse.h>
#include <libutils/Array.h>
#include <libutils/Lock.h>
#include <libutils/Ring.h>

#include "ps2/LegacyDevice.h"

struct LegacyMouse : public LegacyDevice
{
private:
    Lock _events_lock{"legacy-mouse-event"};
    Ring<char> _events{sizeof(MousePacket) * 1024};
    int _cycle = 0;
    Array<uint8_t, 4> _packet;
    bool _quirk_no_mouse_whell = true;

    void wait(int type);

    void write_register(uint8_t a_write);

    uint8_t read_register();

    void handle_finished_packet(uint8_t packet0, uint8_t packet1, uint8_t packet2, uint8_t packet3);

    void handle_packet(uint8_t packet);

public:
    LegacyMouse(DeviceAddress address);

    void handle_interrupt() override;

    bool can_read() override;

    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size);
};
