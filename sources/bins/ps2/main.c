#include <bal/abi.h>
#include <bal/hw.h>
#include <brutal/debug.h>
#include <brutal/input.h>
#include "ps2.h"

typedef struct
{
    BalIo io;

    bool kb_escaped;
} Ps2;

void ps2kb_handle_key(KbKey key, KbMotion motion)
{
    log$("{} {}", kbkey_to_str(key), motion ? "UP" : "DOWN");
}

void ps2kb_handle_code(Ps2 *ps2, uint8_t scancode)
{
    if (ps2->kb_escaped)
    {
        ps2->kb_escaped = false;
        KbKey key = (KbKey)((scancode & 0x7f) + 0x80);
        ps2kb_handle_key(key, scancode & 0x80 ? KBMOTION_UP : KBMOTION_DOWN);
    }
    else if (scancode == PS2_KEYBOARD_ESCAPE)
    {
        ps2->kb_escaped = true;
    }
    else
    {
        KbKey key = (KbKey)(scancode & 0x7f);
        ps2kb_handle_key(key, scancode & 0x80 ? KBMOTION_UP : KBMOTION_DOWN);
    }
}

void ps2_handle_irq(Ps2 *ps2)
{
    uint8_t status = bal_io_in8(ps2->io, PS2_REG_STATUS);

    while (status & PS2_STATUS_FULL)
    {
        uint8_t is_mouse_buf = status & PS2_STATUS_MOUSE;

        if (is_mouse_buf)
        {
            uint8_t packet = bal_io_in8(ps2->io, PS2_REG_BUF);
            log$("MOUSE: {#02x}", packet);
        }
        else
        {
            uint8_t scancode = bal_io_in8(ps2->io, PS2_REG_BUF);
            ps2kb_handle_code(ps2, scancode);
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
        .io = bal_io_port(0x64, 0x8),
        .kb_escaped = false,
    };

    BrCreateArgs keyboard_irq = {
        .type = BR_OBJECT_IRQ,
        .irq = {
            .irq = 1,
        },
    };

    assert_br_success(br_create(&keyboard_irq));

    BrBindArgs keyboard_bind = {
        .handle = keyboard_irq.handle,
        .flags = BR_IRQ_NONE,
    };

    assert_br_success(br_bind(&keyboard_bind));

    BrCreateArgs mouse_irq = {
        .type = BR_OBJECT_IRQ,
        .irq = {
            .irq = 12,
        },
    };

    assert_br_success(br_create(&mouse_irq));

    BrBindArgs mouse_bind = {
        .handle = mouse_irq.handle,
        .flags = BR_IRQ_NONE,
    };

    assert_br_success(br_bind(&mouse_bind));

    BrIpcArgs ipc;
    ipc.flags = BR_IPC_RECV | BR_IPC_BLOCK;
    ipc.deadline = BR_DEADLINE_INFINITY;

    while (br_ipc(&ipc) == BR_SUCCESS)
    {
        BrMsg msg = ipc.msg;

        if (msg.from == BR_TASK_IRQ)
        {
            BrIrq irq = msg.arg[0];
            ps2_handle_irq(&ps2);

            if (irq == 1)
            {
                br_ack(&(BrAckArgs){
                    .handle = keyboard_irq.handle,
                });
            }
            else if (irq == 12)
            {
                br_ack(&(BrAckArgs){
                    .handle = mouse_irq.handle,
                });
            }
        }
        else
        {
            log$("Unknown message!");
        }
    }

    return 0;
}
