#pragma once

#include <bal/hw.h>

/* PS2 8048 driver */
#define PS2_IO_BASE 0x60

typedef enum
{
    PS2_DATA_PORT = 0,
    PS2_STATUS_PORT = 4,
    PS2_COMMAND_PORT = 4,
} Ps2ControllerRegs;

typedef enum
{
    PS2_OUTPUT_STATUS_FULL = 1 << 0, /* 0: empty 1: full */
    PS2_INPUT_STATUS_FULL = 1 << 1,  /* 0: empty 1: full */
    PS2_SYS_FLAG = 1 << 2,
    PS2_DATA_DIRECTION = 1 << 3, /* 0: ps2 device ; 1: ps2 controller */
    PS2_ERROR_TIMEOUT = 1 << 6,
    PS2_ERROR_PARITY = 1 << 7
} Ps2ControllerStatusReg;

typedef enum
{
    PS2_CONTROLLER_CONFIG_READ = 0x20, /* read byte PS2_CONTROLLER_READ + x */
    PS2_CONTROLLER_CONFIG_WRITE = 0x60,
    PS2_SECOND_PORT_DISABLE = 0xA7,
    PS2_SECOND_PORT_ENABLE = 0xA8,
    PS2_SECOND_PORT_TEST = 0xA9,
    PS2_TEST_CONTROLLER = 0xAA,
    PS2_FIRST_PORT8TEST = 0xAB,
    PS2_DUMP = 0xAC,
    PS2_FIRST_PORT_DISABLE = 0xAD,
    PS2_FIRST_PORT_ENABLE = 0xAE,
    PS2_READ_CONTROLLER_INPUT = 0xC0,
    /* why do these exist ? */
    PS2_COPY_LOW_INPUT_TO_HIGH_STATUS = 0xC1,
    PS2_COPY_HIGH_INPUT_TO_LOW_STATUS = 0xC2,
    PS2_READ_CONTROLLER_OUTPUT = 0xD0,
    PS2_WRITE_CONTROLLER_OUTPUT = 0xD1,
    PS2_WRITE_FIRST_PORT_OUTPUT = 0xD2,
    PS2_WRITE_SECOND_PORT_OUTPUT = 0xD3,
    PS2_WRITE_SECOND_PORT_INPUT = 0xD4,
    PS2_DEVICE_RESET = 0xFF,
} Ps2ControllerCommands;

typedef enum
{
    PS2_CFG_FIRST_PORT_INTERRUPT_ENABLE = (1 << 0),
    PS2_CFG_SECOND_PORT_INTERRUPT_ENABLE = (1 << 1),
    PS2_CFG_FIRST_PORT_CLOCK_DISABLE = (1 << 4),
    PS2_CFG_SECOND_PORT_CLOCK_DISABLE = (1 << 5),
    PS2_CFG_FIRST_TRANSLATION = (1 << 6),
} Ps2ControllerConfig;

typedef enum
{
    PS2_OUT_RESET = (1 << 0),
    PS2_OUT_A20 = (1 << 1),
    PS2_OUT_SECOND_PORT_CLOCK = (1 << 2),
    PS2_OUT_SECOND_PORT_DATA = (1 << 3),
    PS2_OUT_FULL_OUTPUT_BUFFER_MOUSE = (1 << 4),
    PS2_OUT_FULL_OUPUT_BUFFER_KEYBOARD = (1 << 5),
    PS2_OUT_FIRST_PORT_CLOCK = (1 << 6),
    PS2_OUT_FIRST_PORT_DATA = (1 << 7),
} Ps2ControllerOutput;

typedef struct
{
    BalIo io;
} Ps2Controller;

uint8_t ps2_controller_read_data(Ps2Controller *self);

void ps2_controller_write_data(Ps2Controller *self, uint8_t data);

void ps2_controller_write_command(Ps2Controller *self, uint8_t command);

void ps2_controller_init(Ps2Controller *self);

uint8_t ps2_controller_status(Ps2Controller *self);

void ps2_second_port_init(Ps2Controller *self);

void ps2_second_port_write(Ps2Controller *self, uint8_t val);

uint8_t ps2_second_port_read(Ps2Controller *self);

void ps2_first_port_init(Ps2Controller *self);

void ps2_first_port_write(Ps2Controller *self, uint8_t val);

void ps2_controller_init(Ps2Controller *self);

void ps2_controller_flush(Ps2Controller *self);
