#include <bal/abi.h>
#include <bal/hw.h>
#include <brutal/debug.h>
#include <brutal/input.h>
#include "ps2.h"

typedef struct
{
    BalIo io;
    bool kb_escaped;

    uint8_t mouse_cycle;
    uint8_t mouse_buf[4];
    bool mouse_has_wheel;
} Ps2;

void ps2_keyboard_handle_key(KbKey key, KbMotion motion)
{
    log$("PS2KB: KEY={} MOTION{}", kbkey_to_str(key), motion ? "UP" : "DOWN");
}

void ps2_mouse_handle_event(MouseEvent event)
{
    log$("PS2MOUSE: OFFX={} OFFY={} VSCROLL={}, BTNS={03b}", event.offx, event.offy, event.vscroll, event.btns);
}

void ps2_keyboard_handle_code(Ps2 *ps2, uint8_t packet)
{
    if (ps2->kb_escaped)
    {
        ps2->kb_escaped = false;
        KbKey key = (KbKey)((packet & 0x7f) + 0x80);
        ps2_keyboard_handle_key(key, packet & 0x80 ? KBMOTION_UP : KBMOTION_DOWN);
    }
    else if (packet == PS2_KEYBOARD_ESCAPE)
    {
        ps2->kb_escaped = true;
    }
    else
    {
        KbKey key = (KbKey)(packet & 0x7f);
        ps2_keyboard_handle_key(key, packet & 0x80 ? KBMOTION_UP : KBMOTION_DOWN);
    }
}

void ps2_mouse_handle_finished(Ps2 *ps2)
{
    uint8_t const *buf = ps2->mouse_buf;

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

    if (ps2->mouse_has_wheel)
    {
        scroll = (int8_t)buf[3];
    }

    // decode the new mouse packet
    MouseEvent event = {};

    event.offx = offx;
    event.offy = -offy;
    event.vscroll = scroll;

    event.btns = 0;
    event.btns |= ((buf[0] >> 0) & 1) ? MSBTN_LEFT : 0;
    event.btns |= ((buf[0] >> 1) & 1) ? MSBTN_RIGHT : 0;
    event.btns |= ((buf[0] >> 2) & 1) ? MSBTN_MIDDLE : 0;

    ps2_mouse_handle_event(event);
}

void ps2_mouse_handle_code(Ps2 *ps2, uint8_t packet)
{
    switch (ps2->mouse_cycle)
    {
    case 0:
        ps2->mouse_buf[0] = packet;
        if (ps2->mouse_buf[0] & 8)
        {
            ps2->mouse_cycle++;
        }
        break;
    case 1:
        ps2->mouse_buf[1] = packet;
        ps2->mouse_cycle++;
        break;
    case 2:
        ps2->mouse_buf[2] = packet;

        if (ps2->mouse_has_wheel)
        {
            ps2->mouse_cycle++;
        }
        else
        {
            ps2_mouse_handle_finished(ps2);
            ps2->mouse_cycle = 0;
        }

        break;
    case 3:
        ps2->mouse_buf[3] = packet;

        ps2_mouse_handle_finished(ps2);
        ps2->mouse_cycle = 0;
        break;
    }
}

void ps2_mouse_wait(Ps2 *ps2, int type)
{
    int timeout = 100000;
    while (timeout--)
    {
        if (type == 0)
        {
            if ((bal_io_in8(ps2->io, PS2_REG_STATUS) & 1) == 1)
            {
                break;
            }
        }
        else
        {
            if ((bal_io_in8(ps2->io, PS2_REG_STATUS) & 2) == 0)
            {
                break;
            }
        }
    }
}

void ps2_mouse_write(Ps2 *ps2, uint8_t data)
{
    ps2_mouse_wait(ps2, 1);
    bal_io_out8(ps2->io, PS2_REG_STATUS, 0xD4);
    ps2_mouse_wait(ps2, 1);
    bal_io_out8(ps2->io, PS2_REG_BUF, data);
}

uint8_t ps2_mouse_read(Ps2 *ps2)
{
    ps2_mouse_wait(ps2, 0);
    return bal_io_in8(ps2->io, PS2_REG_BUF);
}

void ps2_mouse_init(Ps2 *ps2)
{
    // Enable the auxiliary mouse device
    ps2_mouse_wait(ps2, 1);
    bal_io_out8(ps2->io, PS2_REG_STATUS, 0xA8);

    // Enable the interrupts
    ps2_mouse_wait(ps2, 1);
    bal_io_out8(ps2->io, PS2_REG_STATUS, 0x20);
    ps2_mouse_wait(ps2, 0);
    uint8_t status = (bal_io_in8(ps2->io, PS2_REG_BUF) | 3);
    ps2_mouse_wait(ps2, 1);
    bal_io_out8(ps2->io, PS2_REG_STATUS, 0x60);
    ps2_mouse_wait(ps2, 1);
    bal_io_out8(ps2->io, PS2_REG_BUF, status);

    // Tell the mouse to use default settings
    ps2_mouse_write(ps2, 0xF6);
    ps2_mouse_read(ps2); // Acknowledge

    // Enable the mouse
    ps2_mouse_write(ps2, 0xF4);
    ps2_mouse_read(ps2); // Acknowledge

    // Try to  enable the scrollwhell
    ps2_mouse_write(ps2, 0xF2);
    ps2_mouse_read(ps2);

    ps2_mouse_read(ps2);
    ps2_mouse_write(ps2, 0xF3);
    ps2_mouse_read(ps2);
    ps2_mouse_write(ps2, 200);
    ps2_mouse_read(ps2);

    ps2_mouse_write(ps2, 0xF3);
    ps2_mouse_read(ps2);
    ps2_mouse_write(ps2, 100);
    ps2_mouse_read(ps2);

    ps2_mouse_write(ps2, 0xF3);
    ps2_mouse_read(ps2);
    ps2_mouse_write(ps2, 80);
    ps2_mouse_read(ps2);

    ps2_mouse_write(ps2, 0xF2);
    ps2_mouse_read(ps2);

    uint8_t result = ps2_mouse_read(ps2);
    ps2->mouse_has_wheel = result == 3;
    if (ps2->mouse_has_wheel)
    {
        log$("Mouse has a wheel");
    }
    else
    {
        log$("Mouse don't have a wheel");
    }
}

void ps2_handle_irq(Ps2 *ps2)
{
    uint8_t status = bal_io_in8(ps2->io, PS2_REG_STATUS);

    while (status & PS2_STATUS_FULL)
    {
        uint8_t is_mouse_buf = status & PS2_STATUS_MOUSE;

        uint8_t packet = bal_io_in8(ps2->io, PS2_REG_BUF);

        if (is_mouse_buf)
        {
            ps2_mouse_handle_code(ps2, packet);
        }
        else
        {
            ps2_keyboard_handle_code(ps2, packet);
        }

        status = bal_io_in8(ps2->io, PS2_REG_STATUS);
    }
}

int br_entry_args(
    MAYBE_UNUSED long arg1,
    MAYBE_UNUSED long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{
    Ps2 ps2 = {
        .io = bal_io_port(0x60, 0x8),
    };

    BrEvent keyboard_irq = {.type = BR_EVENT_IRQ, .irq = 1};
    BrEvent mouse_irq = {.type = BR_EVENT_IRQ, .irq = 12};

    assert_br_success(br_bind(&(BrBindArgs){.event = keyboard_irq}));
    assert_br_success(br_bind(&(BrBindArgs){.event = mouse_irq}));

    BrIpcArgs ipc;
    ipc.flags = BR_IPC_RECV | BR_IPC_BLOCK;
    ipc.deadline = BR_DEADLINE_INFINITY;

    ps2_mouse_init(&ps2);

    while (br_ipc(&ipc) == BR_SUCCESS)
    {
        BrMsg msg = ipc.msg;

        if (msg.from == BR_TASK_IRQ && msg.event.type == BR_EVENT_IRQ)
        {
            ps2_handle_irq(&ps2);

            br_ack(&(BrAckArgs){.event = msg.event});
        }
        else
        {
            log$("Unknown message!");
        }
    }

    return 0;
}
