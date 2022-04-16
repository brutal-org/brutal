#include <hw/ps2/controller.h>
#include <brutal-debug>

uint8_t ps2_controller_status(Ps2Controller *self)
{
    return bal_io_in8(self->io, PS2_STATUS_PORT);
}

static void ps2_wait_read(Ps2Controller *self)
{
    int timeout = 100000;

    while (timeout--)
    {
        uint8_t status = ps2_controller_status(self);
        if ((status & PS2_OUTPUT_STATUS_FULL) == 1)
        {
            break;
        }
    }

    assert_greater_than(timeout, 0);
}

static void ps2_wait_write(Ps2Controller *self)
{
    int timeout = 100000;

    while (timeout--)
    {
        uint8_t status = ps2_controller_status(self);
        if ((status & PS2_INPUT_STATUS_FULL) == 0)
        {
            break;
        }
    }
    assert_greater_than(timeout, 0);
}

void ps2_controller_write_command(Ps2Controller *self, uint8_t command)
{
    ps2_wait_write(self);
    bal_io_out8(self->io, PS2_COMMAND_PORT, command);
}

uint8_t ps2_controller_read_data(Ps2Controller *self)
{
    ps2_wait_read(self);
    return bal_io_in8(self->io, PS2_DATA_PORT);
}

void ps2_controller_write_data(Ps2Controller *self, uint8_t data)
{
    ps2_wait_write(self);
    bal_io_out8(self->io, PS2_DATA_PORT, data);
}

void ps2_controller_init(Ps2Controller *self)
{
    *self = (Ps2Controller){
        .io = bal_io_port(0x60, 0x8),
    };
}

static uint8_t ps2_controller_read_config(Ps2Controller *self)
{
    ps2_controller_write_command(self, PS2_CONTROLLER_CONFIG_READ);
    return ps2_controller_read_data(self);
}

static void ps2_controller_write_config(Ps2Controller *self, uint8_t data)
{
    ps2_controller_write_command(self, PS2_CONTROLLER_CONFIG_WRITE);
    ps2_controller_write_data(self, data);
}

void ps2_second_port_init(Ps2Controller *self)
{
    ps2_controller_write_command(self, PS2_SECOND_PORT_ENABLE);

    uint8_t status = ps2_controller_read_config(self);
    status |= PS2_CFG_SECOND_PORT_INTERRUPT_ENABLE;
    ps2_controller_write_config(self, status);
}

void ps2_second_port_write(Ps2Controller *self, uint8_t data)
{
    ps2_controller_write_command(self, PS2_WRITE_SECOND_PORT_INPUT);
    ps2_controller_write_data(self, data);
}

uint8_t ps2_second_port_read(Ps2Controller *self)
{
    ps2_controller_write_command(self, PS2_WRITE_SECOND_PORT_OUTPUT);
    return ps2_controller_read_data(self);
}

void ps2_first_port_init(Ps2Controller *self)
{
    ps2_controller_write_command(self, PS2_FIRST_PORT_ENABLE);

    uint8_t status = ps2_controller_read_config(self);
    status |= PS2_CFG_FIRST_PORT_INTERRUPT_ENABLE;
    ps2_controller_write_config(self, status);
}

void ps2_first_port_write(Ps2Controller *self, uint8_t data)
{
    ps2_controller_write_command(self, PS2_WRITE_FIRST_PORT_OUTPUT);
    ps2_controller_write_data(self, data);
}

void ps2_controller_flush(Ps2Controller *self)
{
    uint8_t status = ps2_controller_status(self);

    while (status & PS2_OUTPUT_STATUS_FULL)
    {
        ps2_controller_read_data(self);
        status = ps2_controller_status(self);
    }
}
