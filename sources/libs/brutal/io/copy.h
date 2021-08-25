#pragma once

#include <brutal/io/buffer.h>
#include <brutal/io/read.h>
#include <brutal/io/write.h>

typedef Result(Error, size_t) IoCopyResult;

static inline IoCopyResult io_copy(IoReader *from, IoWriter *to)
{
    size_t total = 0;

    do
    {
        uint8_t buffer[512];
        size_t read = TRY(IoCopyResult, io_read(from, buffer, 512));

        if (read == 0)
        {
            return OK(IoCopyResult, total);
        }

        size_t written = TRY(IoCopyResult, io_write(to, buffer, read));

        if (written == 0)
        {
            return OK(IoCopyResult, total);
        }

        total += written;
    } while (1);
}

static inline Buffer io_readall(IoReader *from, Alloc *alloc)
{
    Buffer buffer;
    buffer_init(&buffer, 512, alloc);
    IoWriter to = buffer_writer(&buffer);
    io_copy(from, &to);
    return buffer;
}
