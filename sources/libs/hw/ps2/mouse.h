#pragma once

#include <hw/ps2/controller.h>
#include <brutal-base>
#include <brutal-ui>

typedef void (*Ps2MouseCallback)(UiEvent ev, void *ctx);

typedef struct
{
    uint8_t cycle;
    uint8_t buf[4];
    bool wheel;
    BrEvent interrupt_handle;
    Ps2MouseCallback callback;
    void *ctx;
    Ps2Controller *controller;
    UiMouseEvent prev;
} Ps2Mouse;

typedef enum
{
    PS2_MOUSE_CMD_RESET = 0xff,
    PS2_MOUSE_CMD_RESEND = 0xfe,
    PS2_MOUSE_CMD_SET_DEFAULT = 0xf6,
    PS2_MOUSE_CMD_DISABLE_REPORT = 0xf5,
    PS2_MOUSE_CMD_ENABLE_REPORT = 0xf4,
    PS2_MOUSE_CMD_SET_RATE = 0xf3,
    PS2_MOUSE_CMD_DEV_ID = 0xf2,
    PS2_MOUSE_CMD_REMOTE_MODE = 0xf0,
    PS2_MOUSE_CMD_WRAP_MODE = 0xee,
    PS2_MOUSE_CMD_WRAP_RESET = 0xec,
    PS2_MOUSE_CMD_READ_DATA = 0xeb,
    PS2_MOUSE_CMD_SET_STREAM_MODE = 0xea,
    PS2_MOUSE_CMD_STATUS_REQUEST = 0xe9,
    PS2_MOUSE_CMD_SET_RESOLUTION = 0xe8,
    PS2_MOUSE_CMD_SET_SCALING = 0xe6,
} Ps2MouseCommands;

void ps2_mouse_init(Ps2Mouse *self, Ps2Controller *controller, Ps2MouseCallback callback, void *ctx);
