#include <brutal/input/keyboard.h>
#include <hw/ps2/keyboard.h>

static void clear_ps2_keyboard_buffer(Ps2Controller* controller)
{
    uint8_t status = ps2_controller_status(controller);

    while (status & PS2_OUTPUT_STATUS_FULL)
    {
        ps2_controller_read_data(controller);
        status = ps2_controller_status(controller);
    }
}

void _ps2_keyboard_init(Ps2Keyboard *self, MAYBE_UNUSED Ps2Controller *controller)
{
    self->kb_escaped = false;
    /* empty already filled in data */
    clear_ps2_keyboard_buffer(controller);
}

void ps2_keyboard_handle_code(Ps2Keyboard *ps2, uint8_t packet)
{
    if (ps2->kb_escaped)
    {
        ps2->kb_escaped = false;
        KbKey key = (KbKey)((packet & 0x7f) + 0x80);
        UiEvent ev = {
            .keyboard = {
                .key = key,
            },
            .type = packet & 0x80 ? UI_EVENT_KEYBOARD_UP : UI_EVENT_KEYBOARD_DOWN,
        };

        ps2->callback(ev, ps2->ctx);
    }
    else if (packet == PS2_KEYBOARD_ESCAPED)
    {
        ps2->kb_escaped = true;
    }
    else
    {
        KbKey key = (KbKey)((packet & 0x7f));

        UiEvent ev = {
            .keyboard = {
                .key = key,
            },
            .type = packet & 0x80 ? UI_EVENT_KEYBOARD_UP : UI_EVENT_KEYBOARD_DOWN,
        };

        ps2->callback(ev, ps2->ctx);
    }
}

bool ps2_keyboard_interrupt_handle(Ps2Keyboard *self, Ps2Controller *controller)
{
    uint8_t status = ps2_controller_status(controller);
    bool updated = false;

    while (status & PS2_OUTPUT_STATUS_FULL && !(status & 0x20))
    {
        uint8_t packet = ps2_controller_read_data(controller);

        ps2_keyboard_handle_code(self, packet);
        status = ps2_controller_status(controller);
        updated = true;
    }

    return updated;
}
