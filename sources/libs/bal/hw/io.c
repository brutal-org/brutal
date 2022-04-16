#include <bal/abi.h>
#include <bal/hw/io.h>
#include <brutal-debug>
#include <brutal-mem>

BalIo bal_io_mem(BalMem *mem)
{
    return (BalIo){BAL_IO_MEMORY, (uintptr_t)mem->buf, mem->len};
}

BalIo bal_io_port(uintptr_t base, size_t len)
{
    return (BalIo){BAL_IO_PORTS, base, len};
}

void bal_io_check(BalIo io, size_t offset, size_t size)
{
    if (offset + size > io.len)
    {
        panic$("Out of bound IO access at offset {#x} with size {}", offset, size);
    }
}

BalIo bal_io_slice(BalIo io, uintptr_t offset, size_t size)
{
    bal_io_check(io, offset, size);

    return (BalIo){io.type, io.base + offset, size};
}

uint8_t bal_io_in8(BalIo io, size_t offset)
{
    bal_io_check(io, offset, 1);

    if (io.type == BAL_IO_PORTS)
    {
        BrIoArgs args = {
            .port = io.base + offset,
            .size = 1,
        };

        br_in(&args);

        return args.data;
    }
    else
    {
        return volatile_read8(io.base + offset);
    }
}

uint16_t bal_io_in16(BalIo io, size_t offset)
{
    bal_io_check(io, offset, 2);

    if (io.type == BAL_IO_PORTS)
    {
        BrIoArgs args = {
            .port = io.base + offset,
            .size = 2,
        };

        br_in(&args);

        return args.data;
    }
    else
    {
        return volatile_read16(io.base + offset);
    }
}

uint32_t bal_io_in32(BalIo io, size_t offset)
{
    bal_io_check(io, offset, 4);

    if (io.type == BAL_IO_PORTS)
    {
        BrIoArgs args = {
            .port = io.base + offset,
            .size = 4,
        };

        br_in(&args);

        return args.data;
    }
    else
    {
        return volatile_read32(io.base + offset);
    }
}

uint64_t bal_io_in64(BalIo io, size_t offset)
{
    bal_io_check(io, offset, 8);

    if (io.type == BAL_IO_PORTS)
    {
        BrIoArgs args = {
            .port = io.base + offset,
            .size = 8,
        };

        br_in(&args);

        return args.data;
    }
    else
    {
        return volatile_read64(io.base + offset);
    }
}

void bal_io_out8(BalIo io, size_t offset, uint8_t data)
{
    bal_io_check(io, offset, 1);

    if (io.type == BAL_IO_PORTS)
    {
        BrIoArgs args = {
            .port = io.base + offset,
            .size = 1,
            .data = data,
        };

        br_out(&args);
    }
    else
    {
        volatile_write8(io.base + offset, data);
    }
}

void bal_io_out16(BalIo io, size_t offset, uint16_t data)
{
    bal_io_check(io, offset, 2);

    if (io.type == BAL_IO_PORTS)
    {
        BrIoArgs args = {
            .port = io.base + offset,
            .size = 2,
            .data = data,
        };

        br_out(&args);
    }
    else
    {
        volatile_write16(io.base + offset, data);
    }
}

void bal_io_out32(BalIo io, size_t offset, uint32_t data)
{
    bal_io_check(io, offset, 4);

    if (io.type == BAL_IO_PORTS)
    {
        BrIoArgs args = {
            .port = io.base + offset,
            .size = 4,
            .data = data,
        };

        br_out(&args);
    }
    else
    {
        volatile_write32(io.base + offset, data);
    }
}

void bal_io_out64(BalIo io, size_t offset, uint64_t data)
{
    bal_io_check(io, offset, 8);

    if (io.type == BAL_IO_PORTS)
    {
        BrIoArgs args = {
            .port = io.base + offset,
            .size = 8,
            .data = data,
        };

        br_out(&args);
    }
    else
    {
        volatile_write64(io.base + offset, data);
    }
}
