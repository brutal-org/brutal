#include <brutal/input.h>
#include <brutal/log.h>
#include <syscalls/io.h>
#include <syscalls/syscalls.h>
#include "ps2.h"

static bool _kb_escaped = false;

void ps2kb_handle_key(KbKey key, KbMotion motion)
{
    log$("{} {}", kbkey_to_str(key), motion ? "UP" : "DOWN");
}

void ps2kb_handle_code(uint8_t scancode)
{
    if (_kb_escaped)
    {
        _kb_escaped = false;
        KbKey key = (KbKey)((scancode & 0x7f) + 0x80);
        ps2kb_handle_key(key, scancode & 0x80 ? KBMOTION_UP : KBMOTION_DOWN);
    }
    else if (scancode == PS2_KEYBOARD_ESCAPE)
    {
        _kb_escaped = true;
    }
    else
    {
        KbKey key = (KbKey)(scancode & 0x7f);
        ps2kb_handle_key(key, scancode & 0x80 ? KBMOTION_UP : KBMOTION_DOWN);
    }
}

void ps2_handle_irq(void)
{
    uint8_t status = br_in8(PS2_STATUS);

    while (status & PS2_BUFFER_FULL)
    {
        uint8_t which_buffer = status & PS2_WHICH_BUFFER;

        if (which_buffer == PS2_KEYBOARD_BUFFER)
        {
            uint8_t scancode = br_in8(PS2_BUFFER);
            ps2kb_handle_code(scancode);
        }
        else if (which_buffer == PS2_MOUSE_BUFFER)
        {
            uint8_t packet = br_in8(PS2_BUFFER);
            log$("MOUSE: {#02x}", packet);
        }

        status = br_in8(PS2_STATUS);
    }
}

int br_entry_args(
    MAYBE_UNUSED long arg1,
    MAYBE_UNUSED long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{
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
            ps2_handle_irq();

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
