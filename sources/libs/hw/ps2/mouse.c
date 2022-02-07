#include <hw/ps2/mouse.h>
#include "ps2/controller.h"

static UiMouseEvent ps2_mouse_status(Ps2Mouse *self)
{
    uint8_t const *buf = self->buf;

    int offx = buf[1];

    if (offx && (buf[0] & (1 << 4)))
    {
        offx -= 0x100;
    }

    int offy = buf[2];

    if (offy && (buf[0] & (1 << 5)))
    {
        offy -= 0x100;
    }

    int scroll = 0;

    if (self->wheel)
    {
        scroll = (int8_t)buf[3];
    }

    // decode the new mouse packet
    UiMouseEvent event = {};

    event.offset.x = offx;
    event.offset.y = -offy;
    event.scroll.y = scroll;

    event.buttons |= ((buf[0] >> 0) & 1) ? MSBTN_LEFT : 0;
    event.buttons |= ((buf[0] >> 1) & 1) ? MSBTN_RIGHT : 0;
    event.buttons |= ((buf[0] >> 2) & 1) ? MSBTN_MIDDLE : 0;

    return event;
}

void ps2_mouse_handle_packet(Ps2Mouse *self, uint8_t packet)
{
    switch (self->cycle)
    {
    case 0:
        self->buf[0] = packet;
        if (self->buf[0] & 8)
        {
            self->cycle++;
        }
        break;
    case 1:
        self->buf[1] = packet;
        self->cycle++;
        break;
    case 2:
        self->buf[2] = packet;

        if (self->wheel)
        {
            self->cycle++;
        }
        else
        {
            self->callback(ps2_mouse_status(self), self->ctx);
            self->cycle = 0;
        }

        break;
    case 3:
        self->buf[3] = packet;
        self->callback(ps2_mouse_status(self), self->ctx);
        self->cycle = 0;
        break;
    }
}

bool ps2_mouse_interrupt_handle(Ps2Mouse *self, Ps2Controller *controller)
{
    uint8_t status = ps2_controller_status(controller);
    bool updated = false;

    while (status & PS2_OUTPUT_STATUS_FULL && status & 0x20)
    {
        uint8_t packet = ps2_controller_read_data(controller);

        ps2_mouse_handle_packet(self, packet);

        status = ps2_controller_status(controller);
        updated = true;
    }

    return updated;
}

void ps2_mouse_send(Ps2Controller* controller, uint8_t data)
{
    ps2_second_port_write(controller, data);
    ps2_controller_read_data(controller);
}

void _ps2_mouse_init(Ps2Mouse* self, Ps2Controller* controller)
{
    ps2_mouse_send(controller, PS2_MOUSE_CMD_SET_DEFAULT );
    ps2_mouse_send(controller, PS2_MOUSE_CMD_ENABLE_REPORT);

    /* enable scrollwheel */
    ps2_mouse_send(controller, 0xF2);
    ps2_controller_read_data(controller);


    ps2_mouse_send(controller, PS2_MOUSE_CMD_SET_RATE);
    ps2_mouse_send(controller, 200);
    ps2_mouse_send(controller, PS2_MOUSE_CMD_SET_RATE);
    ps2_mouse_send(controller, 100);
    ps2_mouse_send(controller, PS2_MOUSE_CMD_SET_RATE);
    ps2_mouse_send(controller, 80);

    ps2_mouse_send(controller, 0xF2);
    ps2_controller_read_data(controller);

    uint8_t status = ps2_controller_read_data(controller);

    self->wheel = status == 3;
}
