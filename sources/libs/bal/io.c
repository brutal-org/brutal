#include <bal/abi.h>

uint8_t br_in8(uint16_t port)
{
    BrIoArgs args = {
        .port = port,
        .size = 1,
    };

    br_in(&args);

    return args.data;
}

uint16_t br_in16(uint16_t port)
{
    BrIoArgs args = {
        .port = port,
        .size = 2,
    };

    br_in(&args);

    return args.data;
}

uint32_t br_in32(uint16_t port)
{
    BrIoArgs args = {
        .port = port,
        .size = 4,
    };

    br_in(&args);

    return args.data;
}

void br_out8(uint16_t port, uint8_t data)
{
    BrIoArgs args = {
        .port = port,
        .size = 1,
        .data = data,
    };

    br_out(&args);
}

void br_out16(uint16_t port, uint16_t data)
{
    BrIoArgs args = {
        .port = port,
        .size = 2,
        .data = data,
    };

    br_out(&args);
}

void br_out32(uint16_t port, uint32_t data)
{
    BrIoArgs args = {
        .port = port,
        .size = 4,
        .data = data,
    };

    br_out(&args);
}
