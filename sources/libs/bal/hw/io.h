#pragma once

#include <brutal/base.h>

typedef enum
{
    BAL_IO_MEMORY,
    BAL_IO_PORTS,
} BalIoType;

typedef struct
{
    BalIoType type;
    size_t base;
    size_t len;
} BalIo;

BalIo bal_io_mem(uintptr_t base, size_t len);

BalIo bal_io_port(uintptr_t base, size_t len);

BalIo bal_io_slice(BalIo io, uintptr_t offset, size_t size);

uint8_t bal_io_in8(BalIo io, size_t offset);

uint16_t bal_io_in16(BalIo io, size_t offset);

uint32_t bal_io_in32(BalIo io, size_t offset);

uint64_t bal_io_in64(BalIo io, size_t offset);

void bal_io_out8(BalIo io, size_t offset, uint8_t data);

void bal_io_out16(BalIo io, size_t offset, uint16_t data);

void bal_io_out32(BalIo io, size_t offset, uint32_t data);

void bal_io_out64(BalIo io, size_t offset, uint64_t data);
